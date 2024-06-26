/**
******************************************************************************
*
* @file       mapripper.cpp
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
* @brief      A class that allows ripping of a selection of the map
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
* @{
*
*****************************************************************************/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include "mapripper.h"
#include <QSettings>
#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QDebug>

namespace mapcontrol
{

MapRipper::MapRipper(internals::Core * core, const internals::RectLatLng & rect):sleep(100),cancel(false),progressForm(0),core(core)
{
    if(!rect.IsEmpty())
    {
        type=core->GetMapType();
        progressForm=new MapRipForm;
        area=rect;
        zoom=core->Zoom();
        maxzoom=core->MaxZoom();
        points=core->Projection()->GetAreaTileList(area,zoom,0);
        progressForm->show();

        //Move the ripper form to the screen center
        this->moveFormToCenter();

        connect(this,SIGNAL(percentageChanged(int)),progressForm,SLOT(SetPercentage(int)));
        connect(this,SIGNAL(numberOfTilesChanged(int,int)),progressForm,SLOT(SetNumberOfTiles(int,int)));
        connect(this,SIGNAL(providerChanged(QString,int)),progressForm,SLOT(SetProvider(QString,int)));
        connect(this,SIGNAL(finished()),this,SLOT(finish()));
        emit numberOfTilesChanged(0,0);

        //Start the ripping when the form button is pressed
        connect(progressForm,SIGNAL(beginRip()),this,SLOT(start()));

        //Stop the current ripping when the form button is pressed
        connect(progressForm,SIGNAL(cancelRip()),this,SLOT(cancelRipping()));

        //Connect to the form to see if should auto rip
        connect(progressForm,SIGNAL(shouldAutoRip(bool)),this,SLOT(setAutoRip(bool)));
    }
}

void MapRipper::moveFormToCenter()
{
    QScreen *pScreen = QGuiApplication::primaryScreen();

    //Grab the current screen size
    QRect screenRec = pScreen->geometry();
    int screenWidth = screenRec.width();
    int screenHeight = screenRec.height();

    //Get and use the static form size
    int formWidth = progressForm->geometry().width();
    int formHeight = progressForm->geometry().height();

    //Move the form to the default center or user saved location
    progressForm->setGeometry((screenWidth/2)-(formWidth/2),(screenHeight/2)-(formHeight/2), formWidth, formHeight);
}

void MapRipper::finish()
{
    if(zoom<maxzoom && cancel==false)
    {
        ++zoom;

        if (shouldAutoRip == true && (zoom <= progressForm->maxAutoRipZoom))
        {
            this->doRip();
        }else{

            if(openMessageBox())
            {
                this->doRip();
            }else{
                this->stopRipping();
            }
        }
    }
    else
    {
        this->stopRipping();
    }
}

void MapRipper::run()
{
    int countOk = 0;
    bool goodtile=false;
    //  Stuff.Shuffle<Point>(ref list);
    QVector<core::MapType::Types> types = OPMaps::Instance()->GetAllLayersOfType(type);
    int all=points.count();
    for(int i = 0; i < all; i++)
    {
        emit numberOfTilesChanged(all,i+1);
        if(cancel)
            break;

        core::Point p = points[i];
        {
            //qDebug()<<"offline fetching:"<<p.ToString();
            foreach(core::MapType::Types type,types)
            {
                emit providerChanged(core::MapType::StrByType(type),zoom);
                QByteArray img = OPMaps::Instance()->GetImageFrom(type, p, zoom);
                if(img.length()!=0)
                {
                    goodtile=true;
                    img=NULL;
                }
                else
                    goodtile=false;
            }
            if(goodtile)
            {
                countOk++;
            }
            else
            {
                i--;
                QThread::msleep(1000);
                continue;
            }
        }
        emit percentageChanged((int) ((i+1)*100/all));//, i+1);
        // worker.ReportProgress((int) ((i+1)*100/all), i+1);

        QThread::msleep(sleep);
    }
}

void MapRipper::doRip()
{
    points.clear();
    points=core->Projection()->GetAreaTileList(area,zoom,0);
    this->start();
}

void MapRipper::stopRipping()
{
    progressForm->close();
    delete progressForm;
    progressForm=NULL;
    this->deleteLater();
}

int MapRipper::openMessageBox()
{
    QMessageBox msgBox;
    msgBox.setText(QString("Continue Ripping at zoom level %1?").arg(zoom));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    bool cont;
    if (ret==QMessageBox::Yes)
        cont = true;
    else
        cont = false;

    return cont;
}

}


