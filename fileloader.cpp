#include "fileloader.h"

FileLoader::FileLoader(QString path){
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    bool flag = false;
    if(path.endsWith("txt", Qt::CaseInsensitive)){
        while(!stream.atEnd()) {
            QString line = stream.readLine();
            if(line.length() > 0 && flag){
                QStringList args = line.split("\t");
                if(args.length() < 10){
                    zero.append(QPair<qreal, QPair<qreal, qreal>>(args.at(2).toDouble(), QPair<qreal, qreal>(args.at(4).toDouble(), args.at(4).toDouble())));
                    bareData.append(QPair<qreal, QPair<qreal, qreal>>(args.at(2).toDouble(), QPair<qreal, qreal>(args.at(3).toDouble(), args.at(3).toDouble())));
                    data.append(QPair<qreal, QPair<qreal, qreal>>(args.at(2).toDouble(), QPair<qreal, qreal>(args.at(6).toDouble(), args.at(6).toDouble())));
                }else{
                    zero.append(QPair<qreal, QPair<qreal, qreal>>(args.at(2).toDouble(), QPair<qreal, qreal>(args.at(4).toDouble(), args.at(8).toDouble())));
                    bareData.append(QPair<qreal, QPair<qreal, qreal>>(args.at(2).toDouble(), QPair<qreal, qreal>(args.at(3).toDouble(), args.at(7).toDouble())));
                    data.append(QPair<qreal, QPair<qreal, qreal>>(args.at(2).toDouble(), QPair<qreal, qreal>(args.at(6).toDouble(), args.at(10).toDouble())));
                }
            }else{
                if(line.length() == 0){
                    flag = true;
                }
            }
        }
        ri = data.length()-1;
    }else if(path.endsWith(".ref", Qt::CaseInsensitive)){
        while(!stream.atEnd()) {
            QString line = stream.readLine();
            if(line.length() > 0){
                QStringList args = line.split("\t");
                if(args.length() == 3){
                    data.append(QPair<qreal, QPair<qreal, qreal>>(args.at(0).toDouble(), QPair<qreal, qreal>(args.at(1).toDouble(), args.at(2).toDouble())));
                }
            }
        }
        ri = data.length()-1;
    }
    file.close();
}

FileLoader::FileLoader(){

}

FileLoader::~FileLoader(){

}

QVector<QPair<qreal, QPair<qreal, qreal>>> FileLoader::getData(){
    QVector<QPair<qreal, QPair<qreal, qreal>>> res;
    for(int i = li; i <= ri; i++){
        res.append(QPair<qreal, QPair<qreal, qreal>>(data.at(i).first + prevShift, data.at(i).second));
    }
    return res;
}

QVector<QPair<qreal, QPair<qreal, qreal>>> FileLoader::getBareData(){
    QVector<QPair<qreal, QPair<qreal, qreal>>> res;
    for(int i = li; i <= ri; i++){
        res.append(QPair<qreal, QPair<qreal, qreal>>(bareData.at(i).first + prevShift, bareData.at(i).second));
    }
    return res;
}

QPair<int, int> FileLoader::getLimits(){
    return QPair<int, int>(li, ri);
}

void FileLoader::setLimits(int nl, int nr, double shift){
    prevShift = shift;
    li = nl;
    ri = data.size() - nr - 1;
}

QVector<QPair<qreal, QPair<qreal, qreal>>> FileLoader::getZero(){
    QVector<QPair<qreal, QPair<qreal, qreal>>> res;
    for(int i = li; i <= ri; i++){
        res.append(QPair<qreal, QPair<qreal, qreal>>(zero.at(i).first + prevShift, zero.at(i).second));
    }
    return res;
}
