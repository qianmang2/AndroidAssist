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
class Callback{
public:
    void callback(QByteArray byteArray);
};

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
    void screenshotCommand(QString serialNumber);
    void callback(QString serialNumber, QString screensRecord, QString filePath, QString fileName);

private:
    QProcess *process = new QProcess();
    QThread *thread= nullptr;
    bool isRecord = false; //是否录屏

signals:
    void onLog(QString log);
};

#endif // UTILS_H
