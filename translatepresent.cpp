#include "translatepresent.h"
#include <QDir>
#include <QTextStream>
#include <QDebug>

TranslatePresent::TranslatePresent(MainWindow *mainWindow, QObject *parent): QObject(parent)
{
    this->mainWindow = mainWindow;
}

void TranslatePresent::insertTranslateTask(QString translatePath, QString projectPath, QString stringIdStr)
{
    int count = 0;
    if(stringIdStr.isNull() || stringIdStr.isEmpty()){
        emit onLog("翻译字符串id为空，任务结束");
        return;
    }
    QStringList stringIds = stringIdStr.split("\n");

    QDir translatePathDir(translatePath);
    QDir projectPathDir(projectPath);
    if(!translatePathDir.exists() || (!translatePath.endsWith("res"))){
        emit onErrorMessage("翻译目录不正确");
        return;
    }
    if(!projectPathDir.exists() || (!projectPath.endsWith("res"))){
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
