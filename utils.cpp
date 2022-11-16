#include "utils.h"
#include "QDateTime"
#include "QCoreApplication"
#include "QDir"
#include "QDebug"
#include "QTimer"

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

QString Utils::exeCommand(QString command, bool isOnLog, QString prefixLog, QString suffixLog)
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

//QString Utils::exeCommand(QString command)
//{
//    //    QThread *thread = new QThread();
//    //    moveToThread(thread);
//    //    thread->start();
//    //    connect(thread, &QThread::started, this, [&](){
//    process->start(command);
//    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [&](int exitCode, QProcess::ExitStatus exitStatus){
//        qDebug() << "exitCode" << exitCode << "exitStatus" << exitStatus;

//    });

//}

void Utils::callback(QString serialNumber, QString screensRecord, QString filePath, QString fileName)
{
    exeCommand("adb -s " + serialNumber + " pull " + screensRecord + " " + filePath, false);
    exeCommand("adb -s " + serialNumber + " shell rm -rf " + screensRecord, false);
    emit onLog("截图保存路径：" + filePath + fileName);
}



QString Utils::exeCommand(QString command, QString log)
{
    process->start(command);
    process->waitForFinished();
    QString result = QString(process->readAllStandardOutput());
    emit onLog(log);
    return result;
}

void Utils::screenshotCommand(QString serialNumber)
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
    QString result = exeCommand("adb -s " + serialNumber + " shell screencap -p " + screenshot, false);
    exeCommand("adb -s " + serialNumber + " pull " + screenshot + " " + filePath, false);
    exeCommand("adb -s " + serialNumber + " shell rm -rf " + screenshot, false);
    emit onLog("截图保存路径：" + filePath + fileName);
}


