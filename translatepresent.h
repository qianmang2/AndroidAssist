#ifndef TRANSLATEPRESENT_H
#define TRANSLATEPRESENT_H

#include <QObject>

class MainWindow;
class TranslatePresent : public QObject
{
    Q_OBJECT
public:
    explicit TranslatePresent(MainWindow *mainWindow, QObject *parent = nullptr);

    void insertTranslateTask(QString translatePath, QString projectPath, QString stringIdStr);

private:
    MainWindow *mainWindow;

signals:
    void onLog(QString log);
    void onErrorMessage(QString errorMessage);

};

#endif // TRANSLATEPRESENT_H
