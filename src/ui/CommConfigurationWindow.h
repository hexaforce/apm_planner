/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of class CommConfigurationWindow
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef _COMMCONFIGURATIONWINDOW_H_
#define _COMMCONFIGURATIONWINDOW_H_

#include <QObject>
#include <QDialog>
#include <QAction>
#include <QPointer>
#include "ProtocolInterface.h"
#include "ui_CommSettings.h"

enum qgc_link_t {
    QGC_LINK_SERIAL,
    QGC_LINK_UDP,
    QGC_LINK_UDP_CLIENT,
    QGC_LINK_TCP,
    QGC_LINK_SIMULATION,
    QGC_LINK_FORWARDING,
#ifdef XBEELINK
    QGC_LINK_XBEE,
#endif
#ifdef OPAL_RT
    QGC_LINK_OPAL
#endif
};

enum qgc_protocol_t {
    QGC_PROTOCOL_MAVLINK
};


#ifdef OPAL_RT
#include "OpalLink.h"
#endif

/**
 * @brief Configuration window for communication links
 */
class CommConfigurationWindow : public QDialog
{
    Q_OBJECT

public:
    CommConfigurationWindow(int linkid, ProtocolInterface* protocol, QWidget *parent = 0);
    ~CommConfigurationWindow();

    QAction* getAction();

    void connectButtonStatus(int linkid);
public slots:
    /** @brief Set the protocol for this link */
    void setProtocol(int protocol);
    void setConnection();
    void connectionState(bool connect);
    void setLinkName(QString name);
    /** @brief Disconnects the associated link, removes it from all menus and closes the window. */
    void remove();
    void linkUpdate(int linkid);
private:

    Ui::commSettings ui;
    int m_linkid;
    QPointer<QAction> action;
};


#endif // _COMMCONFIGURATIONWINDOW_H_
