#include "languagepresent.h"
#include "QDebug"
#include "deviceutil.h"
#include "QMessageBox"
#include "QTimer"
LanguagePresent::LanguagePresent(Ui::MainWindow *ui, QObject *parent) : QObject(parent)
{
    this->ui = ui;
}

LanguagePresent::~LanguagePresent()
{
    if(timer != nullptr){
        if(timer->isActive()){
            timer->stop();
        }
        delete  timer;
    }
}

void LanguagePresent::languageTask()
{
    list.clear();
    Utils::getInstance()->exeCommand("adb root", false);
    QFrame *gridLayout = ui->frame_layout;
    QObjectList children = gridLayout->children();
    for(QObject *obj : children){
        if(QCheckBox *checkBox = dynamic_cast<QCheckBox*>(obj)){
            if(checkBox->isChecked()){
                list.append(checkBox);
            }
        }
    }

    if(!DeviceUtil::getInstance()->isUserable()){
        emit onLog("请连接设备");
        return;
    }

    if(list.isEmpty()) {
        emit onLog("请选择语种");
        return;
    }
    num = list.size();
    if(timer != nullptr){
        timer->stop();
    }

    if(timer == nullptr){
        timer = new QTimer();

        connect(timer, &QTimer::timeout, this, [&](){
            timer->setInterval(3000);
            if(num > 0 && !list.isEmpty()){
                qDebug() << "语言定时器执行中";
                QCheckBox *cb = list.at(list.size() - num);

                QString name = cb->accessibleName();
                QString description = cb->accessibleDescription();
                DevicesBean bean = DeviceUtil::getInstance()->getSelectDevice();
                if(bean.serialNumber.isNull() || bean.serialNumber.isEmpty()){
                    timer->stop();
                    emit onLog("未连接设备");
                    return;
                }
                emit onLog("切换语言至："+ cb->text());
                Utils::getInstance()->exeCommand("adb -s " + bean.serialNumber + " shell am broadcast -n com.qianmang.changelanguage/.ChangeLanguageReceiver -a action.ChangeLanguage --es local \""+ cb->accessibleName() + ","+ cb->accessibleDescription()+"\"", false);

            }else{
                timer->stop();
                emit onLog("所有语种切换完毕");
            }
            num--;
        });
    }
    timer->start();
    timer->setInterval(0);
    qDebug() << list.size();

}
