#ifndef DEVICESBEAN_H
#define DEVICESBEAN_H

#include <QString>

class DevicesBean
{
public:
    DevicesBean();
public:
    QString serialNumber = "";
    QString brand = "";
    QString mode = "";
    QString displayName = "";
    QString state = "";
    bool isUseable = false;
};

#endif // DEVICESBEAN_H
