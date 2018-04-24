#include "calcwrapper.h"

CalcWrapper::CalcWrapper(QObject *parent) : QObject(parent){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
}

CalcWrapper::~CalcWrapper(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
    calculators[0].clear();
    calculators[1].clear();
    emit dead();
}

void CalcWrapper::setRefPaths(QString commonPath){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
    refCommonPath = commonPath;
}

void CalcWrapper::appendCalc(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
    for(int ref = 0; ref < 2; ++ref){
        QSharedPointer<Calculator> p (new Calculator());
        switchConnection(p, calculators[ref].size(), ref);
        calculators[ref].append(p);
    }
}

void CalcWrapper::switchConnection(const QSharedPointer<Calculator> sender, const int id, const int ref){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". id = " + QString::number(id));
    if(ref == 0){
        QObject::connect(sender.data(), &Calculator::rawData, this, [=](const QVector<QPair<qreal, QPointF>>* points, const int file){
            emit rawData(points, id, file);
        });
        QObject::connect(sender.data(), &Calculator::iZero, this, [=](const QVector<QPair<qreal, QPointF>>* points, const int file){
            emit iZero(points, id, file);
        });
        QObject::connect(sender.data(), &Calculator::stepData, this, [=](const QVector<QPointF>* points, const int file){
            emit stepData(points, id, file);
        });
        QObject::connect(sender.data(), &Calculator::moments, this, [=](const qreal* mOP, const qreal* mOO,
                         const qreal* ms, const qreal* mt, const int file){
            emit moments(mOP, mOO, ms, mt, id, file);
        });
    }
    QObject::connect(sender.data(), &Calculator::processedData, this, [=](const QVector<QPair<qreal, QPointF>>* points, const int file){
        emit processedData(points, id, file, ref);
    });
    QObject::connect(sender.data(), &Calculator::XMCD, this, [=](const QVector<QPointF>* points, const int file){
        emit XMCD(points, id, file, ref);
    });
    QObject::connect(sender.data(), &Calculator::integrals, this, [=](const qreal* summ, const qreal* dl2, const qreal* dl3,
                     const qreal* mSE, const qreal* mO, const qreal* rel, const int file){
        emit integrals(summ, dl2, dl3, mSE,  mO, rel, id, file, ref);
    });
    QObject::connect(sender.data(), &Calculator::linCoeffs, this, [=](const QPointF* left, const QPointF* right,
                     const QPointF* x, const int file){
        emit linCoeffs(left, right, x, id, file, ref);
    });
    QObject::connect(sender.data(), &Calculator::completed, this, [=](const int file){
        emit completed(id, file, ref);
    });
    QObject::connect(sender.data(), &Calculator::log, this, [=](QString out){
        emit log("id = " + QString::number(id) + ". ref = " + QString::number(ref) + " :: " + out);
    });
}

void CalcWrapper::removeCalc(const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    for(int ref = 0; ref < 2; ++ref){
        if(id >= 0 && id < calculators[ref].size()){
            calculators[ref].removeAt(id);
            for(int i = id; i < calculators[ref].size(); ++i){
                calculators[ref].at(i).data()->disconnect();
                switchConnection(calculators[ref].at(i), id, ref);
            }
        }else{
            log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id) +
                ". ref = " + QString::number(ref));
        }
    }
}

void CalcWrapper::setLoader(const QString loaderPath, const int file, const int id, const QString refPath){
    if(id >= 0 && id < calculators[0].size()){
        calculators[0].at(id).data()->setLoader(loaderPath, file);
        if(refPath.size() > 0 && refCommonPath.size() > 0){
            calculators[1].at(id).data()->setLoader(refCommonPath + refPath + "/ref.ref", file);
        }else{
            log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". refPath = " + refPath);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setLimits(const qreal left, const qreal right, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        calculators[0].at(id).data()->setLimits(left, right, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setEnergyShift(const qreal shift, const int file, const int id){
    if(id >= 0 && id < calculators[1].size()){
        calculators[1].at(id).data()->setEnergyShift(shift, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setShadowCurrent(const qreal signal, const qreal iZero, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        calculators[0].at(id).data()->setShadowCurrent(signal, iZero, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setSmooth(const int count, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        calculators[0].at(id).data()->setSmooth(count, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setDiff(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->setDiff(needed, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setLinearIntervals(const QPointF interval, const bool needed, const int file, const int id, const int ref){
    if(id >= 0 && id < calculators[ref].size()){
        calculators[ref].at(id).data()->setLinearIntervals(interval, needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setNormalizationCoeff(const qreal coeff, bool needed, const int file, const int id, const int ref){
    if(id >= 0 && id < calculators[ref].size()){
        calculators[ref].at(id).data()->setNormalizationCoeff(coeff, needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setStepped(const qreal coeff, const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->setStepped(coeff, needed, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setIntegrate(const bool needed, const int index, const int file, const int id, const int ref){
    if(id >= 0 && id < calculators[ref].size()){
        calculators[ref].at(id).data()->setIntegrate(needed, index, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setIntegrationConstants(const qreal newPc, const qreal newNh, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->setIntegrationConstants(newPc, newNh);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta, const int id){
    if(id >= 0 && id < calculators[0].size()){
        calculators[0].at(id).data()->setCalculate(needed, newPhi, newTheta);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setLin(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->setLin(needed, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setPositiveIntegrals(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->setIntegratePositiveOnly(needed, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setGround(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->setIntegrateGround(needed, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::setRelativeCurv(const qreal a, const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        calculators[0].at(id).data()->setRelativeCurv(a, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}

void CalcWrapper::update(const int file, const int id){
    if(id >= 0 && id < calculators[0].size()){
        for(int ref = 0; ref < 2; ++ref){
            calculators[ref].at(id).data()->update(file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
}
