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
    setting->setValue(u8"大字体", "adb shell settings put system font_scale 2.0");
    setting->setValue(u8"小字体", "adb shell settings put system font_scale 1.0");
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



