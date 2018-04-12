#include "logger.h"

Logger::Logger(QString path, bool copy, QObject *parent) : QObject(parent){
    if(path.size() != 0){
        logFile = new QFile(path + "/log.txt");
        logFile->open(QIODevice::WriteOnly);
        stream = new QDebug(logFile);
        external = true;
    }
    this->copy = copy;
    print(now.currentDateTime().toString("ddd MMMM d yyyy"));
}

Logger::~Logger(){
    if(external){
        logFile->close();
        delete logFile;
        delete stream;
    }
}

void Logger::print(QString data){
    QString tmp = '[' + now.currentDateTime().toString("hh:mm:ss") + "] " + data;
    if(!external || copy){
        qDebug() << tmp;
    }
    if(external){
        *stream << tmp << endl;
    }
}
