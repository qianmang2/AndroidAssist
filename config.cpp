#include "config.h"
#include "QSettings"
#include "QCoreApplication"
#include <QTextCodec>
#include <QDebug>
Q_GLOBAL_STATIC(Config, instance)
Config::Config(QObject *parent) : QObject(parent)
{
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
    qDebug() << configFile;
    setting = new QSettings(configFile, QSettings::IniFormat);
    setting->setValue("1.显示页面", "adb -s %1 shell \"dumpsys window | grep mCurrentFocus\"");
    setting->setValue("2.大字体", "adb -s %1 shell settings put system font_scale 2.0");
    setting->setValue("3.小字体", "adb -s %1 shell settings put system font_scale 1.0");
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



