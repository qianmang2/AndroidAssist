#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <QObject>
#include <QGlobalStatic>
#include <QSettings>

class ProjectConfig : public QObject
{
    Q_OBJECT
public:
    explicit ProjectConfig(QObject *parent = nullptr);
    static ProjectConfig* getInstance();
    QSettings *setting;
    QStringList keys();
    QVariant value(QString key);
    void setValue(QString key, QString value);

signals:

};

#endif // PROJECTCONFIG_H
