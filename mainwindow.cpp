#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <QStandardItemModel>
#include <config.h>
#include <QFileDialog>
#include <QRegExp>
#include <QMessageBox>
#include <udisk.h>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pbScreenshot= ui->pbScreenshot;
    teLog = ui->teLog;
    cbAdbCommons = ui->cbAdbCommons;
    leSourceFile = ui->leSourceFile;
    leDestFile = ui->leDestFile;
    sourceSelectFile = ui->sourceSelectFile;
    destSelectFile = ui->destSelectFile;
    statusbar = ui->statusbar;

    pDisk = new UDisk;
    qApp->installNativeEventFilter(pDisk);
    initView();
    setStausBar();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete pDisk;
    qApp->removeNativeEventFilter(pDisk);
}

void MainWindow::initView()
{
    connect(pbScreenshot, &QPushButton::clicked, this, [=](){
        if(!isConnectDevice){
            QMessageBox::warning(this, "警告", "请连接设备");
            return;
        }
        Utils::getInstance()->screenshotCommand();
    });
    connect(Utils::getInstance(), &Utils::onLog, [&](QString log){
        teLog->append(log);
    });

    setAdbCommand();
}

void MainWindow::setStausBar()
{
    setStausBarCommand();
    connect(pDisk, &UDisk::deviceChange, this, [&]() {
        QTimer::singleShot(2000, [&](){
            setStausBarCommand();
        });
    });
}

void MainWindow::setStausBarCommand()
{
    QString result = Utils::getInstance()->exeCommand("adb devices", false, false);
    QStringList devices = result.split(QRegExp("\r\n"));
    qDebug() << devices;
    if(devices.length() > 1 && devices.at(1).length() > 1){
        QStringList result = devices.at(1).split(QRegExp("\\t"));
        if(result.length() > 1){
            QString serialNumber = result.at(0);
            statusbar->setToolTip(serialNumber);
        }
        QString brand = Utils::getInstance()->exeCommand("adb shell getprop ro.product.brand", false, false);
        QString model = Utils::getInstance()->exeCommand("adb shell getprop ro.product.model", false, false);
        statusbar->showMessage("已连接设备：" + brand + " " + model);
        isConnectDevice = true;
    }else{
        statusbar->showMessage("未连接设备");
        isConnectDevice = false;
    }
}

void MainWindow::setAdbCommand()
{
    QStringList keys = Config::getInstance()->keys();
    cbAdbCommons->addItems(keys);
    cbAdbCommons->setMaxVisibleItems(8);
    QStandardItemModel *model = (QStandardItemModel*)cbAdbCommons->model();
    for( int i = 0; i < model->rowCount(); i++){
       QStandardItem *item = model->item(i);
        item->setSizeHint(QSize(0, 25));
    }
}


void MainWindow::on_sourceSelectFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,"选择文件");
    if(!file.isNull() && !file.isEmpty()){
        leSourceFile->setText(file);
    }
}

void MainWindow::on_destSelectFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,"选择文件");
    if(!file.isNull() && !file.isEmpty()){
        leDestFile->setText(file);
    }
}

void MainWindow::on_saveFile_clicked()
{
    if(!isConnectDevice){
        QMessageBox::warning(this, "警告", "请连接设备");
        return;
    }
    QString sourceFile = leSourceFile->text();
    QString destFile = leDestFile->text();
    QString command;
    if(sourceFile.contains(QRegExp("^\\w:.*")) && !destFile.contains(QRegExp("^\\w:.*"))){
        command = "adb -s " + statusbar->toolTip() + " push "+ sourceFile + " " + destFile;
        Utils::getInstance()->exeCommand(command, "保存文件成功，保存路径："+ destFile);
    }else if(!sourceFile.contains(QRegExp("^\\w:.*")) && destFile.contains(QRegExp("^\\w:.*"))){
        Utils::getInstance()->exeCommand(command, "保存文件成功，保存路径："+ destFile);
        command = "adb -s " +statusbar->toolTip()+ " pull "+ sourceFile + " " + destFile;
    }else{
        QMessageBox::warning(this, "错误", "文件路径错误，请检查");
    }

}

void MainWindow::on_exeCommand_clicked()
{
    QString commnadName = cbAdbCommons->currentText();
    if(commnadName.isNull() || commnadName.isEmpty()){
        QMessageBox::warning(this, "警告", "暂无配置文件");
        return;
    }
    if(!isConnectDevice){
        QMessageBox::warning(this, "警告", "请连接设备");
        return;
    }
    QString command = Config::getInstance()->value(commnadName).toString();
    Utils::getInstance()->exeCommand(command);
}
