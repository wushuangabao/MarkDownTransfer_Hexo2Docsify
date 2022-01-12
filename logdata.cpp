#include "logdata.h"
#include <QStringList>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
//    QMessageBox msgBox;
//    msgBox.setText("ride remove.");
//    msgBox.exec();
#include <mainwindow.h>


LogData::LogData()
{

}

LogData::~LogData()
{

}

bool LogData::doTransfer(const QString &file_path, QString &title)
{
    QFile file(file_path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return false;
    }

    title = "";
    QString str;
    QString line;
    bool head_is_end = false;
    bool more_is_end = false;

    while (!file.atEnd())
    {
        line = file.readLine();

        // 处理正文
        if(head_is_end)
        {
            if(more_is_end)
                str.append(line);
            else if(line.left(11) == "<!--more-->")
                more_is_end = true;
            else
                str.append(line);
            continue;
        }

        // 处理正文前面的部分（从第一行的“---”到下一个“---”）
        if(line.left(3) == "---")
        {
            if(str != "")
                head_is_end = true;
            str.append("```\n");
        }
        else if(str == "")
        {
            // 如果文件内容的第一行不是“---”，说明这个文件已经处理过了
            if(line.left(2) == "# ")
            {
                title = line.mid(2, line.size() - 3);
                return true;
            }
            else
                return false;
        }
        else if(line.left(7) == "title: ")
        {
            title = line.mid(7, line.size() - 8);
            str.push_front("# " + title + "\n");
        }
        else if(line.left(4) == "toc:")
        {
            str.append("\n");
        }
        else
            str.append(line);
    }

    // 重写文件内容
    file.resize(0);
    file.write(str.toStdString().c_str());
    file.close();

    return true;
}

bool LogData::dealSubTitle(QString &title, QString &sub_title)
{
    if(title[0] == 0x3010/* '【' */)
    {
        int pos = title.indexOf(0x3011/* '】' */); //
        if(pos == -1)
            return false;

        sub_title = title.mid(1, pos - 1);
        title = title.right(title.length() - pos - 1);
        return true;
    }
    else
        return false;
}
