#include "deviceutil.h"

Q_GLOBAL_STATIC(DeviceUtil, instance)
DeviceUtil::DeviceUtil(QObject *parent) : QObject(parent)
{

}

DeviceUtil *DeviceUtil::getInstance()
{
    return instance;
}
