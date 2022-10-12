#include "config.h"
#include "QSettings"
#include "QCoreApplication"
#include <QTextCodec>

Q_GLOBAL_STATIC(Config, instance)
Config::Config(QObject *parent) : QObject(parent)
{
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
    setting = new QSettings(configFile, QSettings::IniFormat);
    setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
}

Config *Config::getInstance()
{
    return instance();
}

QStringList Config::keys()
{
    return setting->allKeys();
}

QVariant Config::value(QString key)
{
    return setting->value(key);
}



