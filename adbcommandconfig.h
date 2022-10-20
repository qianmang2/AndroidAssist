#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QGlobalStatic>
#include <QSettings>
#include <QStringList>

class AdbCommandConfig : public QObject
{
    Q_OBJECT
public:
    explicit AdbCommandConfig(QObject *parent = nullptr);
    static AdbCommandConfig* getInstance();
    QSettings *setting;
    QStringList keys();
    QVariant value(QString key);
    void setValue(QString key, QString value);

signals:

};

#endif // CONFIG_H
