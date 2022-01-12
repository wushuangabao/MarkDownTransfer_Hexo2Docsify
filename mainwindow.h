#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QFile;
class LogData;
struct Data;
typedef void (*Func)();

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
//    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

private:
    void output_File(const QString &path);

    Ui::MainWindow *ui;

    LogData *transforer;

protected:
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);
};

#endif // MAINWINDOW_H
