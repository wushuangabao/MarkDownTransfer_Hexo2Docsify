#ifndef LOGDATA_H
#define LOGDATA_H

#include <QString>

class LogData
{
public:
    LogData();
    ~LogData();

    bool doTransfer(const QString &file_path, QString &title);

    bool dealSubTitle(QString &title, QString &sub_title);
};

#endif // LOGDATA_H
