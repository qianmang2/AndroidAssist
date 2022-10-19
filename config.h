#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QGlobalStatic>
#include <QSettings>
#include <QStringList>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    static Config* getInstance();
    QSettings *setting;
    QStringList keys();
    QVariant value(QString key);
    void setValue(QString key, QString value);

signals:

};

#endif // CONFIG_H
