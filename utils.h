#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QGlobalStatic>
#include <QProcess>
#include <QDateTime>
#include <QDir>
#include <QThread>
#include <QByteArray>
#include <QPushButton>
#include "callback.h"

typedef  void (*CALL_BACK)(QString serialNumber, QString screensRecord, QString filePath, QString fileName);
class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);
    ~Utils();
    static Utils* getInstance();
    QString exeCommand(QString command, bool isOnLog = true, QString prefixLog= nullptr, QString suffixLog = nullptr);
    QString exeCommand(QString command, QString log);
    QString checkCommand(QString command);
    void screenshotCommand(QString serialNumber);
    QString startScreenshotRecording(QString serialNumber,MainWindow *mainWindow, Callback callback);
    void endScreenshotRecording(QString serialNumber,QString fileName, MainWindow *object, Callback callback);

private:
    QProcess *processTask = new QProcess();
    QThread *thread= nullptr;
    bool isRecord = false; //是否录屏

    QProcess *processLongTask = new QProcess();

signals:
    void onLog(QString log);
};

#endif // UTILS_H
