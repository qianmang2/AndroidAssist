#include "udisk.h"
#include <windows.h>
#include <dbt.h>
#include <QDebug>
#include <QDateTime>

UDisk::UDisk(QWidget *parent) : QWidget(parent)
{

}

qint64 lastTime = 0L;
bool UDisk::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        int msgType = msg->message;
        qDebug() << msgType;
        if(msgType == WM_DEVICECHANGE) {// 消息类型， 这里是设备变动；
            QDateTime dateTime = QDateTime::currentDateTime();
            qint64 time = dateTime.toMSecsSinceEpoch();
            if(time - lastTime < 500){
                return false;
            }
            lastTime = time;

            switch (msg->wParam) {
                case DBT_DEVNODES_CHANGED:
                    qDebug() << "DBT_DEVNODES_CHANGED";
                    emit deviceChange();
                    break;
                default:
                    break;
            }
        }
    }
    return false;
}

char UDisk::deviceMask(int unitmask)
{
    char i;

    for (i = 0; i < 26; ++i)
    {
        if (unitmask & 0x1)
            break;
        unitmask = unitmask >> 1;
    }
    return (i + 'A');
}
