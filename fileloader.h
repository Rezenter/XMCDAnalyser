#ifndef FILELOADER_H
#define FILELOADER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QPair>


class FileLoader
{
public:
    FileLoader(QString);
    FileLoader();
    ~FileLoader();
    QVector<QPair<qreal, QPair<qreal, qreal>>> getData();
    QPair<qreal, qreal> getRBackground();
    QPair<qreal, qreal> getLBackground();
    QVector<QPair<qreal, QPair<qreal, qreal>>> getBareData();
    void setLimits(int, int, double);
    QPair<int, int> getLimits();
    QVector<QPair<qreal, QPair<qreal, qreal>>> getZero();

private:
    double prevShift = 0;
    double l = 0;
    double r = 0;
    int li = 0;
    int ri = 0;
    QVector<QPair<qreal, QPair<qreal, qreal>>> data; //QVec<QPair<Energy, QPair<leftPolarisation, rightPolar>>>
    QVector<QPair<qreal, QPair<qreal, qreal>>> bareData;
    QVector<QPair<qreal, QPair<qreal, qreal>>> zero;

};

#endif // FILELOADER_H
