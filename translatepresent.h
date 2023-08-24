#ifndef TRANSLATEPRESENT_H
#define TRANSLATEPRESENT_H

#include <QObject>
#include "ui_mainwindow.h"

class TranslatePresent : public QObject
{
    Q_OBJECT
public:
    explicit TranslatePresent(Ui::MainWindow *ui, QObject *parent = nullptr);

    void append();
private:
    Ui::MainWindow *ui;

signals:
    void onLog(QString log);
    void onErrorMessage(QString errorMessage);

};

#endif // TRANSLATEPRESENT_H
