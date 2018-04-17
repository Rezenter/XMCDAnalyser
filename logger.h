#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QDateTime>


class Logger : public QObject{

    Q_OBJECT

public:
    Logger(QString path, bool copy, QObject *parent = 0);
    ~Logger();

private:
    QFile* logFile;
    QDebug* stream;
    QDateTime now;
    bool external = false;
    bool copy = true;

public slots:
    void print(QVariant data);
};

#endif // LOGGER_H
