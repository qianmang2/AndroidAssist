#include "translatepresent.h"
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QDomElement>
#include <QDomNode>

TranslatePresent::TranslatePresent(Ui::MainWindow *ui, QObject *parent): QObject(parent)
{
    this->ui = ui;
}

void TranslatePresent::append()
{
    QString translatePath = ui->leTranslatePath->text();
    QString projectPath =  ui->leProjectPath->text();
    QString stringIdStr =  ui->translateStringId->toPlainText();
    if(stringIdStr.isNull() || stringIdStr.isEmpty()){
        emit onLog("翻译字符串id为空，任务结束");
        return;
    }

    QStringList tempStringId = stringIdStr.split("\n");
    QStringList stringIds;
    foreach(QString s, tempStringId){ //去掉前后空白符
        stringIds << s.trimmed();
    }

    QDir translatePathDir(translatePath);
    QDir projectPathDir(projectPath);
    if(!translatePathDir.exists() || (!translatePath.endsWith("res") && !translatePath.endsWith("res\\"))){
        emit onLog("翻译目录不正确");
        return;
    }
    if(!projectPathDir.exists() || (!projectPath.endsWith("res") && !projectPath.endsWith("res\\"))){
        emit onLog("工程目录不正确");
        return;
    }
    int count = 0;
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

        QFile projectFile(projectPath + "/" + dir + "/strings.xml");

        QList<QDomElement> targetElement;
        if (!translateFile.open(QIODevice::ReadOnly))
            return;
        emit onLog("--------开始处理" + dir + "目录--------");
        QDomDocument translateDoc;
        translateDoc.setContent(&translateFile);
        QDomElement docElem = translateDoc.documentElement();
        QDomNode n = docElem.firstChild();
        bool isInsert = false;
        while (!n.isNull()) {
            QDomElement e = n.toElement(); //将节点转换为元素
            if (!e.isNull()) {
                qDebug() <<e.tagName();
                qDebug() << e.attribute("name");
                if(stringIds.contains(e.attribute("name"))){
                    targetElement.append(e);
                    emit onLog(dir + "增加" + e.attribute("name") + "成功");
                    isInsert = true;
                }

            }
            n = n.nextSibling();
        }
        if(targetElement.isEmpty()){
            continue;
        }
        if(isInsert){
            count++;
        }
        translateFile.close();

        if (!projectFile.open(QIODevice::ReadWrite)){
            return;
        }

        QDomDocument projectDoc;
        projectDoc.setContent(&projectFile);
        projectFile.resize(0);
        QDomElement projectElem = projectDoc.documentElement();
        if(projectElem.isNull()){
            projectElem = projectDoc.createElement("resource");
            projectElem.setAttribute("xmlns:android", "http://schemas.android.com/apk/res/android");
            projectElem.setAttribute("xmlns:xliff", "urn:oasis:names:tc:xliff:document:1.2");
            projectElem.setAttribute("xmlns:tools", "http://schemas.android.com/tools");
            projectElem.setAttribute("tools:ignore", "MissingTranslation");
            projectDoc.appendChild(projectElem);
        }

        QDomNode lastChild = projectDoc.lastChild();

        foreach(QDomElement dom, targetElement){
            lastChild.appendChild(dom);
        }
        QTextStream stream( &projectFile );
        stream.setCodec("utf-8");
        projectDoc.save(stream,4,QDomNode::EncodingFromTextStream);
        projectFile.close();
        emit onLog(QString("统计:已成功插入%1种语言").arg(count));
    }

}
