#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QStatusBar>
#include <udisk.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_sourceSelectFile_clicked();

    void on_destSelectFile_clicked();

    void on_saveFile_clicked();

    void on_exeCommand_clicked();

    void on_pbInsertString_clicked();

    void on_pbTranslatePath_clicked();

    void on_pbProjectPath_clicked();

    void handleTranslateTask();

private:
    void initView();
    void setStausBar();
    void setStausBarCommand();
    void setAdbCommand();
    QStringList readProjectContent();
private:
    Ui::MainWindow *ui;
    QPushButton *pbScreenshot;
    QTextEdit *teLog;
    QComboBox *cbAdbCommons;
    QLineEdit *leSourceFile;
    QLineEdit *leDestFile;
    QPushButton *sourceSelectFile;
    QPushButton *destSelectFile;
    QStatusBar *statusbar;
    bool isConnectDevice = false;
    QLineEdit *leTranslatePath;
    QLineEdit *leProjectPath;
    QTextEdit *translateStringId;
    UDisk *pDisk = nullptr;
    QThread *thread = nullptr;
signals:
    void onLog(QString log);
    void onErrorMessage(QString errorMessage);

};
#endif // MAINWINDOW_H
