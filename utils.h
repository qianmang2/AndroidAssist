#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QGlobalStatic>
#include <QProcess>
#include <QDateTime>
#include <QDir>
#include <QThread>

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

private:
    QProcess *process = new QProcess();
    QThread *thread= nullptr;


signals:
    void onLog(QString log);
};

#endif // UTILS_H
