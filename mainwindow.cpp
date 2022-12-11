#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <QStandardItemModel>
#include <adbcommandconfig.h>
#include <QFileDialog>
#include <QRegExp>
#include <QMessageBox>
#include <udisk.h>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <deviceutil.h>
#include <projectconfig.h>
#include <QPalette>
#include <QDesktopServices>
#include <QProgressDialog>

#define TRANSLATE_PATH "translatePath"
#define PROJECT_PATH "projectPath"

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
    pbScreenRecord = ui->pbScreenRecord;
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

    if(translatePresent != nullptr){
        delete translatePresent;
    }

    if(progress != nullptr){
        delete  progress;
    }
}

void MainWindow::initView()
{
    connect(pbScreenshot, &QPushButton::clicked, this, [=](){
        if(!DeviceUtil::getInstance()->isConnected){
            QMessageBox::warning(this, "警告", "请连接设备");
            return;
        }
        Utils::getInstance()->screenshotCommand(DeviceUtil::getInstance()->serialNumber);
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
    QPushButton *help = new QPushButton("使用帮助", statusbar);
    connect(help, &QPushButton::clicked, this, []{
        QDesktopServices::openUrl(QUrl("https://github.com/qianmang2/AndroidAssist"));
    });
    QPalette p;
    help->setFlat(true);
    help->setAttribute(Qt::WA_TranslucentBackground);
    p.setColor(QPalette::ButtonText, Qt::blue);
    help->setPalette(p);
    statusbar->addPermanentWidget(help);

    connect(pDisk, &UDisk::deviceChange, this, [&]() {
        QTimer::singleShot(1000, [&](){
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
            QString status = result.at(1);
            DeviceUtil::getInstance()->serialNumber = serialNumber;
            QString brand = Utils::getInstance()->exeCommand("adb -s " + serialNumber + " shell getprop ro.product.brand", false);
            QString model = Utils::getInstance()->exeCommand("adb -s " + serialNumber + " shell getprop ro.product.model", false);
            if(brand.isNull() || brand.isEmpty()) {
                statusbar->showMessage("设备连接异常：" + status);
                DeviceUtil::getInstance()->isConnected = false;
            }else{
                statusbar->showMessage("已连接设备：" + brand + " " + model);
                DeviceUtil::getInstance()->isConnected = true;
            }
        }
    }else{
        statusbar->showMessage("未连接设备");
        DeviceUtil::getInstance()->serialNumber = nullptr;
        DeviceUtil::getInstance()->isConnected = false;
    }

}

void MainWindow::setAdbCommand()
{
    QStringList keys = AdbCommandConfig::getInstance()->keys();
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
    if(!DeviceUtil::getInstance()->isConnected){
        QMessageBox::warning(this, "警告", "请连接设备");
        return;
    }
    QString sourceFile = leSourceFile->text();
    QString destFile = leDestFile->text();
    QString command;
    QString log;
    if(sourceFile.contains(QRegExp("^\\w:.{2,}")) && !destFile.contains(QRegExp("^\\w:.*"))){
        command = "adb -s " + DeviceUtil::getInstance()->serialNumber + " push "+ sourceFile + " " + destFile;
        log = Utils::getInstance()->exeCommand(command, false);
    }else if(!sourceFile.contains(QRegExp("^\\w:.*")) && destFile.contains(QRegExp("^\\w:.{2,}"))){
        command = "adb -s " + DeviceUtil::getInstance()->serialNumber + " pull "+ sourceFile + " " + destFile;
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
    if(!DeviceUtil::getInstance()->isConnected){
        QMessageBox::warning(this, "警告", "请连接设备");
        return;
    }
    QString command = AdbCommandConfig::getInstance()->value(commnadName).toString().arg(DeviceUtil::getInstance()->serialNumber);
    Utils::getInstance()->exeCommand(command.arg(DeviceUtil::getInstance()->serialNumber));
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
    QString defaultDir = AdbCommandConfig::getInstance()->value(TRANSLATE_PATH).toString();
    QString dir = QFileDialog::getExistingDirectory(this, "选择目录", defaultDir);
    if(!dir.isNull() && !dir.isEmpty()){
        leTranslatePath->setText(dir);
        ProjectConfig::getInstance()->setValue(TRANSLATE_PATH, dir);
    }
}

void MainWindow::on_pbProjectPath_clicked()
{
    QString defaultDir = AdbCommandConfig::getInstance()->value(PROJECT_PATH).toString();
    QString dir = QFileDialog::getExistingDirectory(this, "选择目录", defaultDir);
    if(!dir.isNull() && !dir.isEmpty()){
        leProjectPath->setText(dir);
        ProjectConfig::getInstance()->setValue(PROJECT_PATH, dir);
    }
}

void MainWindow::handleTranslateTask()
{
    if(translatePresent == nullptr){
        translatePresent = new TranslatePresent(this);
        connect(translatePresent, &TranslatePresent::onLog, [&](QString log){
            teLog->append(log);
        });

        connect(translatePresent, &TranslatePresent::onErrorMessage, this, [&](QString errorMessage){
            QMessageBox::warning(this, "警告", errorMessage);
        });

    }
    QString translatePath = leTranslatePath->text();
    QString projectPath = leProjectPath->text();
    QString stringIdStr = translateStringId->toPlainText();
    translatePresent->insertTranslateTask(translatePath, projectPath, stringIdStr);
}


void MainWindow::startRecordCallback()
{
    pbScreenRecord->setText("停止录屏");
    pbScreenRecord->setEnabled(true);
}

void MainWindow::endRecordCallback(){
    pbScreenRecord->setText("开始录屏");
    pbScreenRecord->setEnabled(true);
    dismissProgress();
}

void MainWindow::showProgress(QString title, QString label)
{
    if(progress == nullptr){
        progress = new QProgressDialog(this);
        progress->setValue(0);
        progress->setMinimumDuration(1);
        progress->setMinimumWidth(300);
        progress->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::CustomizeWindowHint);
        progress->setWindowModality(Qt::WindowModal);
        progress->setRange(0,0);
    }
    progress->setCancelButton(nullptr);
    progress->setWindowTitle(title);
    progress->setLabelText(label);
    progress->show();
}

void MainWindow::dismissProgress()
{
    if(progress != nullptr){
        progress->close();
    }
}

QString recordFileName;
void MainWindow::on_pbScreenRecord_clicked()
{
    if(isRecording){
        showProgress();
        pbScreenRecord->setEnabled(false);
        Utils::getInstance()->endScreenshotRecording(DeviceUtil::getInstance()->serialNumber, recordFileName, this, &MainWindow::endRecordCallback);

    }else{
        pbScreenRecord->setEnabled(false);
        recordFileName = Utils::getInstance()->startScreenshotRecording(DeviceUtil::getInstance()->serialNumber, this, &MainWindow::startRecordCallback);
    }
    isRecording = !isRecording;
}
