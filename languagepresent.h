#ifndef LANGUAGEPRESENT_H
#define LANGUAGEPRESENT_H

#include <QObject>
#include <utils.h>
#include "ui_mainwindow.h"

class LanguagePresent : public QObject
{
    Q_OBJECT
public:
    explicit LanguagePresent(Ui::MainWindow *ui, QObject *parent = nullptr);
    ~LanguagePresent();
    void languageTask();

private:
    Ui::MainWindow *ui;
    QList<QCheckBox*> list;
     QTimer *timer = nullptr;
     int num = 0;

signals:
    void onLog(QString log);
};

#endif // LANGUAGEPRESENT_H
