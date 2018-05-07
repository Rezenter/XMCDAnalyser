#include "RefCalculator.h"

RefCalculator::RefCalculator(QObject *parent) : QObject(parent){

}

RefCalculator::~RefCalculator(){
    loader[0].~FileLoader();
    loader[1].~FileLoader();
}

void RefCalculator::setLoader(const QString loaderPath, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". path = " + loaderPath);
    setOffset(true);
    if(path[file] != loaderPath){
        path[file] = loaderPath;
        loaderChanged[file] = true;
        reset();
    }
    setOffset();
}

void RefCalculator::setEnergyShift(const qreal shift, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". shift = " +
        QString::number(shift));
    setOffset(true);
    if(shift != tmpEnergyShift[file]){
        tmpEnergyShift[file] = shift;
        loaderChanged[file] = true;
        reset();
    }
    setOffset();
}

void RefCalculator::load(const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file));
    setOffset(true);
    loader[file].setLimits(0, 0, energyShift[file]);
    data[file].resize(loader[file].getData().size());
    if(data[file].size() > 0){
        for(int i = 0; i <  data[file].size(); ++i){
            QPair<qreal, QPair<qreal, qreal>> tmp = loader[file].getData().at(i);
            data[file][i] = QPair< qreal, QPointF>(tmp.first, QPointF(tmp.second.first, tmp.second.second));
        }
        loaded[file] = true;
        if(active[file]){
            normalize(file);
        }
    }else{
        log("ref is not loaded!");
    }
    setOffset();
}

void RefCalculator::setNormalizationCoeff(const qreal coeff, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". coeff = " +
        QString::number(coeff));
    setOffset(true);
    if(tmpNormalizationCoeff[file][0] != coeff){
        tmpNormalizationCoeff[file][0] = coeff;
        normalizationChanged[file] = true;
        reset();
    }
    setOffset();
}

void RefCalculator::normalize(const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file));
    setOffset(true);
    qreal x1 = data[file].first().first;
    qreal x2 = data[file].last().first;
    normalizationCoeff[file][1] = (normalizationCoeff[file][0]*(qPow(x1, 2.0) - qPow(x2, 2.0)))/(x2 - x1);
    normalizationCoeff[file][2] = 1 - normalizationCoeff[file][1]*x1 - normalizationCoeff[file][0]*qPow(x1, 2.0);
    normData[file].resize(data[file].size());
    for(int i = 0; i < data[file].size(); i++){
        normData[file][i].first = data[file][i].first;
        normData[file][i].second.rx() = data[file][i].second.x()*(normalizationCoeff[file][0]*qPow(data[file][i].first, 2.0) +
                normalizationCoeff[file][1]*data[file][i].first + normalizationCoeff[file][2]);
        normData[file][i].second.ry() = data[file][i].second.y()*(normalizationCoeff[file][0]*qPow(data[file][i].first, 2.0) +
                normalizationCoeff[file][1]*data[file][i].first + normalizationCoeff[file][2]);
    }
    diff[file].resize(normData[file].size());
    lEdges[file] = 0;
    rEdges[file] = 0;
    for(int i = 0; i < normData[file].size(); ++i){
        diff[file][i] = QPointF(normData[file][i].first, normData[file][i].second.y() - normData[file][i].second.x());
        if(diff[file][i].y() < diff[file][lEdges[file]].y()){
            lEdges[file] = i;
        }else if(diff[file][i].y() > diff[file][rEdges[file]].y()){
            rEdges[file] = i;
        }
    }
    emit processedData(&normData[file], file);
    emit XMCD(&diff[file], file);
    stepped(file);
    setOffset();
}

void RefCalculator::setStepped(const qreal coeff, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". coeff = " +
        QString::number(coeff));
    setOffset(true);
    if(tmpSteppedCoeff[file] != coeff){
        tmpSteppedCoeff[file] = coeff;
        steppedCoeffChanged[file] = true;
        reset();
    }
    setOffset();
}

void RefCalculator::stepped(const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file));
    setOffset(true);
    qreal m = ((normData[file].last().second.x() - normData[file][0].second.x()) +
                (normData[file].last().second.y() - normData[file][0].second.y()))/(6 * M_PI);
    finalData[file].resize(normData[file].size());
    for(int i = 0; i < normData[file].size(); i++){
        qreal step = (normData[file][0].second.x() + normData[file][0].second.y())/2.0 +
                            m * (M_PI_2 + qAtan(steppedCoeff[file]*(normData[file][i].first - normData[file][lEdges[file]].first))) * 2 +
                            m * (M_PI_2 + qAtan(steppedCoeff[file]*(normData[file][i].first - normData[file][rEdges[file]].first)));
        finalData[file][i] = QPair<qreal, QPointF>(normData[file][i].first,
                                                   QPointF(normData[file][i].second.x() - step, normData[file][i].second.y() - step));
    }
    emit processedData(&finalData[file], file);
    integrate(file);
    setOffset();
}

void RefCalculator::setIntegrate(const qreal index, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". index = " +
        QString::number(index));
    setOffset(true);
    if(tmpSeparator[file] != index){
        tmpSeparator[file] = index;
        integrationChanged[file] = true;
        reset();
    }else{
        log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file)  + ". index = " +
            QString::number(index));
    }
    setOffset();
}

void RefCalculator::setIntegratePositiveOnly(const bool needed, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". needed = " +
        QString::number(needed));
    setOffset(true);
    if(needed != tmpIntegratePositiveOnly[file]){
        tmpIntegratePositiveOnly[file] = needed;
        integrationChanged[file] = true;
        reset();
    }
    setOffset();
}

void RefCalculator::setIntegrateGround(const bool needed, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". needed = " +
        QString::number(needed));
    setOffset(true);
    if(needed != tmpIntegrateGround[file]){
        tmpIntegrateGround[file] = needed;
        integrationChanged[file] = true;
        reset();
    }
    setOffset();
}

void RefCalculator::integrate(const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file));
    setOffset(true);
    summInt[file] = 0.0;
    for(int i = 0; i < finalData[file].size() - 1; i++){
        summInt[file] += (finalData[file][i + 1].first - finalData[file][i].first)*
                (finalData[file][i + 1].second.x() + finalData[file][i].second.x() +
                 finalData[file][i + 1].second.y() + finalData[file][i].second.y())/2.0;
    }
    QVector<QPointF> finalDiff;
    finalDiff.resize(diff[file].size());
    if(this->separator[file] == 0.0 && diff[file].size() > 2){
        this->separator[file] = diff[file][qFloor(diff[file].size()/2)].x();
    }
    int separator = 0;
    int shift = 0;
    for(int i = 0; i < diff[file].size(); ++i){
        finalDiff[i + shift] = diff[file][i];
        if((i < (diff[file].size() - 1)) && (diff[file][i].x() <= this->separator[file]) && (diff[file][i + 1].x() > this->separator[file])){
            separator = i;
            if(diff[file][i].x() != this->separator[file]){
                separator++;
                shift = 1;
                finalDiff.insert(shift + i, QPointF(this->separator[file], diff[file][i].y() + (diff[file][i + 1].y() - diff[file][i].y())
                                 *(this->separator[file] - diff[file][i].x())/(diff[file][i + 1].x() - diff[file][i].x())));
            }
        }
    }
    qreal a = (finalDiff[0].y() - finalDiff[separator].y())/
            (finalDiff[0].x() - finalDiff[separator].x());
    qreal b = finalDiff[0].y() - a*finalDiff[0].x();
    dl3Int[file] = 0.0;
    for(int i = 0; i < separator; i++){
        qreal trap = ((finalDiff[i+1].x() - finalDiff[i].x())*
                (finalDiff[i+1].y() + finalDiff[i].y())/2.0);
        if(integrateGround[file]){
            trap -= (finalDiff[i+1].x() - finalDiff[i].x())*
                    (a*(finalDiff[i+1].x() + finalDiff[i].x())/2.0 + b);
        }
        if(!integratePositiveOnly[file] || trap < 0.0){
            dl3Int[file] += trap;
        }
    }
    a = (finalDiff[separator].y() - finalDiff[finalData[file].size() - 1].y())/
            (finalDiff[separator].x() - finalDiff[finalData[file].size() - 1].x());
    b = finalDiff[separator].y() - a*finalDiff[separator].x();
    dl2Int[file] = 0.0;
    for(int i = separator; i < finalData[file].size() - 1; i++){
        qreal trap = ((finalDiff[i+1].x() - finalDiff[i].x())*
                (finalDiff[i+1].y() + finalDiff[i].y())/2.0);
        if(integrateGround[file]){
            trap -= (finalDiff[i+1].x() - finalDiff[i].x())*
                    (a*(finalDiff[i+1].x() + finalDiff[i].x())/2.0 + b);
        }
        if(!integratePositiveOnly[file] || trap > 0.0){
            dl2Int[file] += trap;
        }
    }
    emit integrals(&summInt[file], &dl2Int[file], &dl3Int[file], file);
    ready = true;
    reset();
    setOffset();
}



void RefCalculator::reset(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
    if(ready){
        for(int file = 0; file < 2; file++){
            if(loaderChanged[file]){
                loaderChanged[file] = false;
                loader[file] = FileLoader(path[file]);
                energyShift[file] = tmpEnergyShift[file];
                loaded[file] = false;
                load(file);
            }else if(loaded && active[file]){
                if(normalizationChanged[file]){
                    normalizationChanged[file] = false;
                    normalizationCoeff[file][0] = tmpNormalizationCoeff[file][0]*1E-4;
                    normalize(file);
                }else if(steppedCoeffChanged[file]){
                    steppedCoeffChanged[file] = false;
                    steppedCoeff[file] = tmpSteppedCoeff[file];
                    stepped(file);
                }else if(integrationChanged[file]){
                    integrationChanged[file] = false;
                    separator[file] = tmpSeparator[file];
                    integratePositiveOnly[file] = tmpIntegratePositiveOnly[file];
                    integrateGround[file] = tmpIntegrateGround[file];
                    integrate(file);
                }
            }
            emit completed(file);
        }
    }
    setOffset();
}

void RefCalculator::update(const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file));
    setOffset(true);
    loaderChanged[file] = true;
    reset();
    setOffset();
}

void RefCalculator::activateRef(const bool state, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". file = " + QString::number(file) + ". state = " +
        QString::number(state));
    setOffset(true);
    if(state != active[file]){
        active[file] = state;
        update(file);
    }
    log("activated");
    setOffset();
}
