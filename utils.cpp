#include "utils.h"
#include "QDateTime"
#include "QCoreApplication"
#include "QDir"
#include "QDebug"
#include "QTimer"
#include "QGlobalStatic"


Q_GLOBAL_STATIC(Utils, instance)
Utils::Utils(QObject *parent) : QObject(parent)
{

}

Utils::~Utils()
{
    delete processTask;
    delete processLongTask;
}

Utils *Utils::getInstance()
{
    return instance;
}

QString Utils::exeCommand(QString command, bool isOnLog, QString prefixLog, QString suffixLog)
{
    qDebug() << command;
    QProcess *process = new QProcess();
    process->start(command);
    QEventLoop loop;
    connect(process, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    loop.exec();
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
    process->close();
    delete process;
    return log;
}

QString Utils::exeCommand(QString command, QString log)
{
    qDebug() << command;
    QProcess *process = new QProcess();
    process->start(command);
    QEventLoop loop;
    connect(process, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    loop.exec();
    QString result = QString(process->readAllStandardOutput());
    emit onLog(log);
    process->close();
    delete process;
    return result;
}

QString Utils::checkCommand(QString command)
{
    processLongTask->start(command);
    QEventLoop loop;
    connect(processLongTask, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    loop.exec();
    QString log = QString(processLongTask->readAllStandardOutput());
    return log;
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

QString Utils::startScreenshotRecording(QString serialNumber,MainWindow *mainWindow, Callback callback)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString fileName =  "Screenshot_" + dateTime.toString("yy_MM_dd_HHmmss") + ".mp4";
    QString screenshot = "/sdcard/" + fileName;
    processTask->start("adb -s " + serialNumber + " shell screenrecord " + screenshot);
    (mainWindow->*callback)();
    emit onLog("开始录屏");
    return fileName;
}


void Utils::endScreenshotRecording(QString serialNumber,QString fileName,MainWindow *mainWindow, Callback callback)
{
    processTask->close();
    QTimer::singleShot(500, [=](){
        qDebug() << QThread::currentThreadId();
        QString filePath = QCoreApplication::applicationDirPath();
        QDir dir(filePath);
        if(!dir.exists("screen")){
            dir.mkdir("screen");
        }
        filePath = filePath + "/screen/";
        exeCommand("adb -s " + serialNumber + " pull /sdcard/" + fileName + " " + filePath, false);
        exeCommand("adb -s " + serialNumber + " shell rm -rf /sdcard/" + fileName, false);
        (mainWindow->*callback)();
        emit onLog("录屏保存路径：" + filePath + fileName);
    });
}

