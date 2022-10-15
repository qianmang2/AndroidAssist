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
#include <QThread>

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
    leTranslatePath = ui->leTranslatePath;
    leProjectPath = ui->leProjectPath;
    translateStringId = ui->translateStringId;

    pDisk = new UDisk;
    qApp->installNativeEventFilter(pDisk);
    initView();
    setStausBar();
}

MainWindow::~MainWindow()
{
    delete ui;

    if(pDisk != nullptr){
        delete pDisk;
    }
    qApp->removeNativeEventFilter(pDisk);
    if(thread != nullptr){
        delete thread;
    }
}

void MainWindow::initView()
{
    connect(pbScreenshot, &QPushButton::clicked, this, [=](){
        if(!isConnectDevice){
            QMessageBox::warning(this, "警告", "请连接设备");
            return;
        }
        Utils::getInstance()->screenshotCommand(statusbar->toolTip());
    });
    connect(Utils::getInstance(), &Utils::onLog, [&](QString log){
        teLog->append(log);
    });

    connect(teLog, &QTextEdit::textChanged, this, [&](){
        teLog->moveCursor(QTextCursor::End);
    });

    connect(this, &MainWindow::onLog, this, [&](QString log){
        teLog->append(log);
    });
    connect(this, &MainWindow::onErrorMessage, this, [&](QString errorMessage){
        QMessageBox::warning(this, "警告", errorMessage);
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
    QString result = Utils::getInstance()->exeCommand("adb devices", false);
    QStringList devices = result.split(QRegExp("\r\n"));
    qDebug() << devices;
    if(devices.length() > 1 && devices.at(1).length() > 1){
        QStringList result = devices.at(1).split(QRegExp("\\t"));
        if(result.length() > 1){
            QString serialNumber = result.at(0);
            statusbar->setToolTip(serialNumber);
            QString brand = Utils::getInstance()->exeCommand("adb -s " + serialNumber + " shell getprop ro.product.brand", false);
            QString model = Utils::getInstance()->exeCommand("adb -s " + serialNumber + " shell getprop ro.product.model", false);
            statusbar->showMessage("已连接设备：" + brand + " " + model);
            isConnectDevice = true;
        }
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
    QString log;
    if(sourceFile.contains(QRegExp("^\\w:.{2,}")) && !destFile.contains(QRegExp("^\\w:.*"))){
        command = "adb -s " + statusbar->toolTip() + " push "+ sourceFile + " " + destFile;
        log = Utils::getInstance()->exeCommand(command, false);
    }else if(!sourceFile.contains(QRegExp("^\\w:.*")) && destFile.contains(QRegExp("^\\w:.{2,}"))){
        command = "adb -s " + statusbar->toolTip() + " pull "+ sourceFile + " " + destFile;
        log = Utils::getInstance()->exeCommand(command, false);
    } else{
        QMessageBox::warning(this, "错误", "文件路径错误，请检查");
    }
    if(log.contains("error: failed")){
        QMessageBox::warning(this, "错误", "文件路径错误，请检查");
    }else{
        teLog->append("保存文件成功，保存路径：" + destFile);
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
    QString command = Config::getInstance()->value(commnadName).toString().arg(statusbar->toolTip());
    Utils::getInstance()->exeCommand(command);
}

//插入翻译字符串
void MainWindow::on_pbInsertString_clicked()
{
    thread = new QThread();
    connect(thread, &QThread::started, this, &MainWindow::handleTranslateTask);
    thread->start();
}

void MainWindow::on_pbTranslatePath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择目录");
    if(!dir.isNull() && !dir.isEmpty()){
        leTranslatePath->setText(dir);
    }
}

void MainWindow::on_pbProjectPath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择目录");
    if(!dir.isNull() && !dir.isEmpty()){
        leProjectPath->setText(dir);
    }
}

void MainWindow::handleTranslateTask()
{
    int count = 0;
    QString translatePath = leTranslatePath->text();
    QString projectPath = leProjectPath->text();
    QString stringIdStr = translateStringId->toPlainText();
    if(stringIdStr.isNull() || stringIdStr.isEmpty()){
        emit onLog("翻译字符串id为空，任务结束");
        return;
    }
    QStringList stringIds = stringIdStr.split("\n");

    QDir translatePathDir(translatePath);
    QDir projectPathDir(projectPath);
    if(!translatePathDir.exists() || (!translatePath.endsWith("/res") && !translatePath.endsWith("/res/"))){
        emit onErrorMessage("翻译目录不正确");
        return;
    }
    if(!projectPathDir.exists() || (!projectPath.endsWith("/res") && !projectPath.endsWith("/res/"))){
        emit onErrorMessage("工程目录不正确");
        return;
    }
    QStringList translatePathDirs = translatePathDir.entryList(QDir::Dirs);
    QStringList projectPathDirs = projectPathDir.entryList(QDir::Dirs);
    foreach(QString dirName, translatePathDirs){
        if(!dirName.startsWith("values")){
            translatePathDirs.removeOne(dirName); //清除非value目录
        }
    }

    for(int i = 0 ; i < translatePathDirs.size(); i++){
        QString dir = translatePathDirs.at(i);
        QFile translateFile(translatePath + "/"+dir + "/strings.xml" );
        if(!translateFile.exists()){
            continue;
        }
        if(!projectPathDirs.contains(dir)){
            if(!projectPathDir.exists(dir)){
                projectPathDir.mkdir(dir);
            }
        }
        QString projectAll="";
        QFile projectFile(projectPath + "/" + dir + "/strings.xml");
        if(!projectFile.exists()){
            projectFile.open(QIODevice::ReadWrite|QIODevice::Text);
            projectAll.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
                    .append("\n")
                    .append("<resources xmlns:android=\"http://schemas.android.com/apk/res/android\">");
        }
        projectFile.open(QIODevice::ReadWrite|QIODevice::Text);
        QTextStream projectTextStream(&projectFile);
        while(!projectTextStream.atEnd()){
            projectAll.append(projectTextStream.readAll());
        }
        projectFile.close();
        projectAll.replace("</resources>","");
        projectAll = projectAll.trimmed();
        translateFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream textStream(&translateFile);
        emit onLog("--------开始处理" + dir + "目录--------");
        bool isInsert = false;
        while(!textStream.atEnd()){
            QString line = textStream.readLine();
            qDebug() << line;
            foreach(QString stringId, stringIds){
                if(line.contains("<string name=\"" + stringId+ "\"") && !projectAll.contains(line)){
                    projectAll.append("\n").append( line);
                    isInsert = true;
                    emit onLog(dir + "增加" + stringId + "成功");
                }
            }
        }
        if(isInsert){
            count++;
        }
        projectAll.append("\n").append("</resources>");
        projectFile.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate);
        projectTextStream<< projectAll;
        translateFile.close();
        projectFile.close();
    }
    qDebug()<<count;
    emit onLog(QString("统计:已成功插入%1%2").arg(count).arg("种语言"));
}
