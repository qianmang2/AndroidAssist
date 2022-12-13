#include "deviceutil.h"

Q_GLOBAL_STATIC(DeviceUtil, instance)
DeviceUtil::DeviceUtil(QObject *parent) : QObject(parent)
{

}

DeviceUtil::~DeviceUtil()
{
    delete devices;
}


DeviceUtil *DeviceUtil::getInstance()
{
    return instance;
}

bool DeviceUtil::isUserable()
{
    return selectDevice.isUseable;
}

void DeviceUtil::clear()
{
    devices->clear();
}

void DeviceUtil::addDevice(DevicesBean device)
{
    devices->append(device);
}

QStringList DeviceUtil::getDisplayNames()
{
    QStringList displayName;
    for(DevicesBean bean : *devices){
        displayName << bean.displayName;
    }
    return displayName;
}

void DeviceUtil::setSelectDevice(DevicesBean device)
{
    selectDevice = device;
}

DevicesBean DeviceUtil::getSelectDevice()
{
    return selectDevice;
}

DevicesBean DeviceUtil::getDeviceBean(int index)
{
    if(index >= 0 && index < devices->size()) {
        return devices->at(index);
    }
    return DevicesBean();
}

QStringList DeviceUtil::getSerialNumbers(){
    QStringList serialNumbers;
    for(DevicesBean bean : *devices) {
        serialNumbers << bean.serialNumber;
    }
    return serialNumbers;
}

void DeviceUtil::remove(QString serialNumber)
{
    for(int i = 0 ; i < devices->size(); i++){
        DevicesBean bean = devices->at(i);
        if(bean.serialNumber == serialNumber){
            devices->removeAt(i);
            break;
        }
    }
}

int DeviceUtil::indexOf(QString serialNumber)
{
    for(int i = 0 ; i < devices->size(); i++){
        DevicesBean bean = devices->at(i);
        if(bean.serialNumber == serialNumber){
            return i;
        }
    }
    return -1;
}
