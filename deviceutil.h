#ifndef DEVICEUTIL_H
#define DEVICEUTIL_H

#include <QObject>
#include <QGlobalStatic>
#include <QList>
#include <devicesbean.h>

class DeviceUtil : public QObject
{
    Q_OBJECT
public:

    explicit DeviceUtil(QObject *parent = nullptr);
    ~DeviceUtil();
    static DeviceUtil* getInstance();
    bool isUserable();
    void clear();
    void addDevice(DevicesBean device);
    QStringList getDisplayNames();
    DevicesBean getSelectDevice();
    DevicesBean getDeviceBean(int index);
    void setSelectDevice(DevicesBean device);
    QStringList getSerialNumbers();
    void remove(QString serialNumber);
    int indexOf(QString serialNumber);
    QList<DevicesBean> *devices = new QList<DevicesBean>();
private:

    QString serialNumber;
    DevicesBean selectDevice;



signals:

};

#endif // DEVICEUTIL_H
