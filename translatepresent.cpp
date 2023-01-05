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
        qDebug() << "filename=" <<translateFile.fileName();
        translateFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream textStream(&translateFile);
        QString translateAll = textStream.readAll();
        bool hasContainStringId = true;
        foreach(QString stringId, stringIds){
            QString type1 = "<string name=\"" + stringId+ "\"";
            QString type2 = "<plurals name=\"" + stringId+ "\"";
            hasContainStringId &= translateAll.contains(type1) || translateAll.contains(type2);
        }
        if(!hasContainStringId){
            continue;
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


        emit onLog("--------开始处理" + dir + "目录--------");
        bool isInsert = false;
        textStream.resetStatus();
        textStream.seek(0);
        QString newInsertString= "";
        while(!textStream.atEnd()){
            QString line = textStream.readLine();
            qDebug() << line;
            foreach(QString stringId, stringIds){
                if(line.contains("<string name=\"" + stringId+ "\"") && !projectAll.contains(line)){
                    newInsertString.append( line).append("\n");
                    if(!line.contains("</string>")){
                        while(true){
                            QString line = textStream.readLine();
                            newInsertString.append( line).append("\n");
                            if(line.contains("</string>")){
                                break;
                            }
                        }
                    }


                    isInsert = true;
                    emit onLog(dir + "增加" + stringId + "成功");
                }else if(line.contains("<plurals name=\"" + stringId+ "\"") && !projectAll.contains(line)){
                    newInsertString.append( line).append("\n");
                    while(true){
                        QString line = textStream.readLine();
                        newInsertString.append( line).append("\n");
                        if(line.contains("</plurals>")){
                            break;
                        }
                    }
                    isInsert = true;
                    emit onLog(dir + "增加" + stringId + "成功");
                }
            }
        }
        if(isInsert){
            count++;
        }
        int index = projectAll.lastIndexOf("</resources>");
        projectAll.insert(index, newInsertString);
        projectFile.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate);
        projectTextStream<< projectAll;
        translateFile.close();
        projectFile.close();
    }
    qDebug()<<count;
    emit onLog(QString("统计:已成功插入%1%2").arg(count).arg("种语言"));
}
