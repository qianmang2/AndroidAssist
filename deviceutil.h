#ifndef DEVICEUTIL_H
#define DEVICEUTIL_H

#include <QObject>
#include <QGlobalStatic>

class DeviceUtil : public QObject
{
    Q_OBJECT
public:
    explicit DeviceUtil(QObject *parent = nullptr);
    static DeviceUtil* getInstance();

public:
    bool isConnected;
    QString serialNumber;

signals:

};

#endif // DEVICEUTIL_H
