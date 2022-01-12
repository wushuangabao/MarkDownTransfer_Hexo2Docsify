#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logdata.h"
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QtDebug>
#include <QDragEnterEvent>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    transforer(new LogData)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete transforer;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();	//放行，否则不会执行dropEvent()函数
}

void MainWindow::dropEvent(QDropEvent *e)
{
    // 获取文件路径 (QString)
    QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty()) return;
    QString str = urls.first().toLocalFile();

    ui->lineEdit_Path->setText(str);

//    // 转为char*
//    QByteArray qByteArrary = str.toLatin1();
//    char* filePath = qByteArrary.data();
}

// 点击“浏览”按钮
//void MainWindow::on_pushButton_clicked()
//{
//    QString fileName = QFileDialog::getOpenFileName(this, "选择路径", "", "Log files (*.txt *.log)");
//    ui->lineEdit_Path->setText(fileName);
//}

void MainWindow::on_pushButton_4_clicked()
{
    QString file_path = ui->lineEdit_Path->text();
    file_path = file_path.replace('\\','/');

    int i = file_path.size() - 1;
    QString file_root;
    if(file_path[i] == '/')
        file_root= file_path.left(i);

    i = file_root.lastIndexOf('/') + 1;
    if(-1 == i) return;
    QString last_layer = file_root.right(file_root.size() - i);

    while(last_layer != "wushuangabao.github.io")
    {
        i = file_root.lastIndexOf('/');
        if(-1 == i) return;
        file_root = file_root.left(i);

        i = file_root.lastIndexOf('/') + 1;
        if(-1 == i) return;
        last_layer = file_root.right(file_root.size() - i);
    }
    ui->lineEdit_Path->setText(file_root);

    // 现在的 file_root 是类似 "E:/github/wushuangabao.github.io" 的路径
    // 在该路径（目录）下有如下子目录：
    QString read_layer  = "doc/read";   //读书文摘
    QString think_layer = "doc/think";  //所思所想
    QString code_layer  = "doc/code";   //编程艺术
    QStringList layer_list = {read_layer, think_layer, code_layer};

    // 新写一个_sidebar.md
    QFile file(file_root + "/_sidebar.md");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.resize(0);
    file.write("**[网站导航](doc/guide)**\n\n");

    // 存储 title 的数据结构（【】里的内容提取到 sub_layers）
    QStringList sub_layers = {"其他"};
    QVector<QStringList*> titles;
    QVector<QStringList*> paths;
    titles.append(new QStringList());
    paths.append(new QStringList());

    // 遍历、处理三个子目录中的.md 文件
    int idx = 0;
    for(; idx < 3; idx++)
    {
        if(idx == 0)
            file.write("- 读书文摘\n");
        else if(idx == 1)
            file.write("- 所思所想\n");
        else if(idx == 2)
            file.write("- 编程艺术\n");

        QString layer_root = file_root + "/" + layer_list.at(idx);
        QDir dir(layer_root);
        if(!dir.exists())
            continue;

        dir.setFilter(QDir::Files);
        QStringList list = dir.entryList();
        i = 0;
        while(i < list.size())
        {
            if(list.at(i).contains(".md"))
            {
                QString title;
                QString f = layer_root + "/" + list.at(i);

                // 读取标题并对文件格式做调整
                bool b = transforer->doTransfer(f, title);
                if(b)
                {
                    // 若成功，将标题先存下来
                    QString sub_title;
                    if(transforer->dealSubTitle(title, sub_title))
                    {
                        if(sub_layers.contains(sub_title) == false)
                        {
                            sub_layers.append(sub_title);

                            QStringList* str_list_t = new QStringList();
                            str_list_t->append(title);
                            titles.append(str_list_t);

                            QStringList* str_list_p = new QStringList();
                            str_list_p->append(list.at(i));
                            paths.append(str_list_p);
                        }
                        else
                        {
                            int pos = sub_layers.indexOf(sub_title);
                            titles[pos]->append(title);
                            paths[pos]->append(list.at(i));
                        }
                    }
                    else
                    {
                        titles[0]->append(title);
                        paths[0]->append(list.at(i));
                    }
                }
            }
            ++i;
        }

        // 将标题写入_sidebar.md
        if(sub_layers.size() == 1)
        {
            QString str;
            int n = titles[0]->size();
            for(int j = 0; j < n; j++)
            {
                str = "\t- [" + titles[0]->at(j) + "](" + layer_list.at(idx) + "/" + paths[0]->at(j) + ")\n";
                file.write(str.toStdString().c_str());
            }
        }
        else
        {
            for(int i = sub_layers.size() - 1; i >= 0; i--)
            {
                file.write(("\t- " + sub_layers[i] + "\n").toStdString().c_str());
                QString str;
                int n = titles[i]->size();
                for(int j = 0; j < n; j++)
                {
                    str = "\t\t- [" + titles[i]->at(j) + "](" + layer_list.at(idx) + "/" + paths[i]->at(j) + ")\n";
                    file.write(str.toStdString().c_str());
                }
            }
        }

        // 清空数据，准备下一轮
        for(i = titles.size() - 1; i >= 0; i--)
        {
            titles[i]->clear();
            paths[i]->clear();
        }
        titles.clear();
        paths.clear();
        sub_layers.clear();
        titles.append(new QStringList());
        paths.append(new QStringList());
        sub_layers.append("其他");

        // 结束遍历子目录
    }

    file.write("\n**关于**\n- [作者](about/me)");
    file.close();
}
