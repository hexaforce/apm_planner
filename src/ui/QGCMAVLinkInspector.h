#ifndef QGCMAVLINKINSPECTOR_H
#define QGCMAVLINKINSPECTOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QMap>
#include <QTimer>

#include "MAVLinkProtocol.h"

namespace Ui {
    class QGCMAVLinkInspector;
}

class QTreeWidgetItem;
class UASInterface;

class QGCMAVLinkInspector : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMAVLinkInspector(QWidget *parent = nullptr);
    ~QGCMAVLinkInspector() override;

public slots:
    void receiveMessage(LinkInterface* link,mavlink_message_t message);
    /** @brief Clear all messages */
    void clearView();
    /** @brief Update view */
    void refreshView();
    /** @brief Add system to the list */
    void addSystem(UASInterface* uas);
    /** @brief Add component to the list */
    void addComponent(int uas, int component, const QString& name);
    /** @Brief Select a system through the drop down menu */
    void selectDropDownMenuSystem(int dropdownid);
    /** @Brief Select a component through the drop down menu */
    void selectDropDownMenuComponent(int dropdownid);

    void rateTreeItemChanged(QTreeWidgetItem* paramItem, int column);

private:
    MAVLinkProtocol *_protocol {nullptr};     ///< MAVLink instance
    int selectedSystemID {0};           ///< Currently selected system
    int selectedComponentID {0};        ///< Currently selected component
















    QMap<int, int> systems;         ///< Already observed systems
    QMap<int, int> components;      ///< Already observed components
    QMap<int, float> onboardMessageInterval; ///< Stores the onboard selected data rate
    QMap<int, QTreeWidgetItem*> rateTreeWidgetItems; ///< Available rate tree widget items
    QTimer updateTimer; ///< Only update at 1 Hz to not overload the GUI
    QHash<quint32, mavlink_message_info_t> messageInfo; ///< Meta information about all messages


    QMap<int, QTreeWidgetItem* > uasTreeWidgetItems; ///< Tree of available uas with their widget


    QMap<int, QMultiMap<int, QTreeWidgetItem*>* > uasMsgTreeItems; ///< Stores the widget of the received message for each UAS

    QMultiMap<int, mavlink_message_t* > uasMessageStorage; ///< Stores the messages for every UAS

    QMultiMap<int, QMap<int, float>* > uasMessageHz; ///< Stores the frequency of each message of each UAS
    QMultiMap<int, QMap<int, unsigned int>* > uasMessageCount; ///< Stores the message count of each message of each UAS

    QMultiMap<int, QMap<int, quint64>* > uasLastMessageUpdate; ///< Stores the time of the last message for each message of each UAS

    /* @brief Update one message field */
    void updateField(int sysid, int msgid, int fieldid, QTreeWidgetItem* item);
    /** @brief Rebuild the list of components */
    void rebuildComponentList();
    /** @brief Change the stream interval */
    void changeStreamInterval(int msgid, int interval);
    /* @brief Create a new tree for a new UAS */
    void addUAStoTree(int sysId);

    static constexpr unsigned int updateInterval {1000}; ///< The update interval of the refresh function
    static constexpr float updateHzLowpass {0.2f}; ///< The low-pass filter value for the frequency of each message


    Ui::QGCMAVLinkInspector *mp_Ui;
};

#endif // QGCMAVLINKINSPECTOR_H
