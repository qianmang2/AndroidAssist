#include "projectconfig.h"
#include <QCoreApplication>
#include <QTextCodec>

Q_GLOBAL_STATIC(ProjectConfig, instance)
ProjectConfig::ProjectConfig(QObject *parent) : QObject(parent)
{
    setting = new QSettings("androidAssist.ini", QSettings::IniFormat);
    setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
}

ProjectConfig *ProjectConfig::getInstance()
{
    return instance();
}

QStringList ProjectConfig::keys()
{
    return setting->allKeys();
}

QVariant ProjectConfig::value(QString key)
{
    return setting->value(key);
}

void ProjectConfig::setValue(QString key, QString value)
{
    setting->setValue(key, value);
}
