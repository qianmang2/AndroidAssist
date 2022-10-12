#include "utils.h"
#include "QDateTime"
#include "QCoreApplication"
#include "QDir"

Q_GLOBAL_STATIC(Utils, instance)
Utils::Utils(QObject *parent) : QObject(parent)
{

}

Utils::~Utils()
{
    delete process;
}

Utils *Utils::getInstance()
{
    return instance;
}

QString Utils::exeCommand(QString command, bool isOnLog, bool isOnCommand, QString prefixLog, QString suffixLog)
{
    process->start(command);
    process->waitForFinished();
    QString log = QString(process->readAllStandardOutput());
    if(isOnLog){
        QString resultLog = log.trimmed();
        if(prefixLog != nullptr){
            resultLog = prefixLog + resultLog;
        }
        if(suffixLog != nullptr){
            resultLog = resultLog + suffixLog;
        }
        emit onLog(resultLog);
      }
    return log;

}

QString Utils::exeCommand(QString command, QString log)
{
    process->start(command);
    process->waitForFinished();
    QString result = QString(process->readAllStandardOutput());
    emit onLog(log);
    return result;
}

void Utils::screenshotCommand()
{
    QString filePath = QCoreApplication::applicationDirPath();
    QDir dir(filePath);
    if(!dir.exists("screen")){
        dir.mkdir("screen");
    }
    QDateTime dateTime = QDateTime::currentDateTime();
    filePath = filePath + "/screen/";
    QString fileName =  "Screenshot_" + dateTime.toString("yy_MM_dd_HHmmss") + ".png";
    QString screenshot = "/sdcard/" + fileName;
    exeCommand("adb shell screencap -p " + screenshot, false);
    exeCommand("adb pull " + screenshot + " " + filePath, false);
    exeCommand("adb shell rm -rf " + screenshot, false);
    emit onLog("截图保存路径：" + filePath + fileName);
}
