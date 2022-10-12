#ifndef UDISK_H
#define UDISK_H

#include <QWidget>
#include<QAbstractNativeEventFilter>

class UDisk : public QWidget,public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit UDisk(QWidget *parent = nullptr);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    char deviceMask(int unitmask);

signals:
    void deviceChange();
};

#endif // UDISK_H
