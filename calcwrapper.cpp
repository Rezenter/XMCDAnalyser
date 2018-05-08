#include "calcwrapper.h"

CalcWrapper::CalcWrapper(QObject *parent) : QObject(parent){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
}

CalcWrapper::~CalcWrapper(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
    setOffset(true);
    calculators.clear();
    setOffset();
    emit dead();
}

void CalcWrapper::setRefPaths(QString commonPath){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
    setOffset(true);
    refCommonPath = commonPath;
    setOffset();
}

void CalcWrapper::appendCalc(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".");
    setOffset(true);
    QPair<Calculator*, RefCalculator*> pair(new Calculator(), new RefCalculator());
    calculators.append(pair);
    switchConnection(pair, calculators.size() - 1);
    setOffset();
}

void CalcWrapper::switchConnection(const QPair<Calculator*, RefCalculator*> senderPair, const int newId){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". id = " + QString::number(newId));
    setOffset(true);
    QObject::connect(senderPair.first, &Calculator::rawData, this,
                     [=](const QVector<QPair<qreal, QPointF>>* points, const int file){
        emit rawData(points, newId, file);
    });
    QObject::connect(senderPair.first, &Calculator::iZero, this, [=](const QVector<QPair<qreal, QPointF>>* points, const int file){
        emit iZero(points, newId, file);
    });
    QObject::connect(senderPair.first, &Calculator::stepData, this, [=](const QVector<QPointF>* points, const int file){
        emit stepData(points, newId, file);
    });
    QObject::connect(senderPair.first, &Calculator::moments, this, [=](const qreal* mOP, const qreal* mOO,
                     const qreal* ms, const qreal* mt, const int file){
        emit moments(mOP, mOO, ms, mt, newId, file);
    });
    QObject::connect(senderPair.first, &Calculator::processedData, this,
                     [=](const QVector<QPair<qreal, QPointF>>* points, const int file){
        emit processedData(points, newId, file, 0);
    });
    QObject::connect(senderPair.first, &Calculator::XMCD, this, [=](const QVector<QPointF>* points, const int file){
        emit XMCD(points, newId, file, 0);
    });
    QObject::connect(senderPair.first, &Calculator::integrals, this, [=](const qreal* summ, const qreal* dl2, const qreal* dl3,
                     const qreal* mSE, const qreal* mO, const qreal* rel, const int file){
        emit integrals(summ, dl2, dl3, mSE,  mO, rel, newId, file, 0);
    });
    QObject::connect(senderPair.first, &Calculator::linCoeffs, this, [=](const QPointF* left, const QPointF* right,
                     const QPointF* x, const int file){
        emit linCoeffs(left, right, x, newId, file);
    });
    QObject::connect(senderPair.first, &Calculator::completed, this, [=](const int file){
        emit completed(newId, file, 0);
    });
    QObject::connect(senderPair.first, &Calculator::log, this, [=](QVariant out){
        emit log("id = " + QString::number(newId) + ". ref = " + QString::number(0) + " :: " + out.toString());
    });
    QObject::connect(senderPair.first, &Calculator::setOffset, this, [=](bool state){
        emit setOffset(state);
    });
    QObject::connect(senderPair.second, &RefCalculator::processedData, this, [=](const QVector<QPair<qreal, QPointF>>* points,
                     const int file){
        emit processedData(points, newId, file, 1);
    });
    QObject::connect(senderPair.second, &RefCalculator::XMCD, this, [=](const QVector<QPointF>* points, const int file){
        emit XMCD(points, newId, file, 1);
    });
    QObject::connect(senderPair.second, &RefCalculator::integrals, this, [=](const qreal* summ, const qreal* dl2, const qreal* dl3,
                     const int file){
        emit integrals(summ, dl2, dl3, nullptr,  nullptr, nullptr, newId, file, 1);
    });
    QObject::connect(senderPair.second, &RefCalculator::completed, this, [=](const int file){
        emit completed(newId, file, 1);
    });
    QObject::connect(senderPair.second, &RefCalculator::log, this, [=](QVariant out){
        emit log("id = " + QString::number(newId) + ". ref = " + QString::number(1) + " :: " + out.toString());
    });
    QObject::connect(senderPair.second, &RefCalculator::setOffset, this, [=](bool state){
        emit setOffset(state);
    });
    setOffset();
}

void CalcWrapper::removeCalc(const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->disconnect();
        calculators.at(id).second->disconnect();
        calculators.removeAt(id);
        log(calculators.size());
        for(int i = id; i < calculators.size(); ++i){
            log(i);
            calculators.at(i).first->disconnect();
            calculators.at(i).second->disconnect();
            switchConnection(calculators.at(i), i);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setLoader(const QString loaderPath, const int file, const int id, const QString refPath){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setLoader(loaderPath, file);
        log("calculator alive");
        if(refPath.size() > 0 && refCommonPath.size() > 0){
            log("loading reference");
            calculators.at(id).second->setLoader(refCommonPath + refPath + "/ref.ref", file);
            log("reference alive");
        }else{
            log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". refPath = " + refPath);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setLimits(const qreal left, const qreal right, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setLimits(left, right, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setEnergyShift(const qreal shift, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).second->setEnergyShift(shift, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setShadowCurrent(const qreal signal, const qreal iZero, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setShadowCurrent(signal, iZero, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setSmooth(const int count, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setSmooth(count, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setDiff(const bool needed, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setDiff(needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setLinearIntervals(const QPointF interval, const bool needed, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setLinearIntervals(interval, needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setNormalizationCoeff(const qreal coeff, bool needed, const int file, const int id, const int ref){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        if(ref == 0){
            calculators.at(id).first->setNormalizationCoeff(coeff, needed, file);
        }else if(ref == 1){
            calculators.at(id).second->setNormalizationCoeff(coeff, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setStepped(const qreal coeff, const bool needed, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setStepped(coeff, needed, file);
        if(needed){
            calculators.at(id).second->setStepped(coeff, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setIntegrate(const bool needed, const qreal index, const int file, const int id, const int ref){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        if(ref == 0){
            calculators.at(id).first->setIntegrate(needed, (int)index, file);
        }else if(ref == 1 && needed){
            calculators.at(id).second->setIntegrate(index, file);
        }
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setIntegrationConstants(const qreal newPc, const qreal newNh, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
            calculators.at(id).first->setIntegrationConstants(newPc, newNh);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setCalculate(needed, newPhi, newTheta);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setLin(const bool needed, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setLin(needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setPositiveIntegrals(const bool needed, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setIntegratePositiveOnly(needed, file);
        calculators.at(id).second->setIntegratePositiveOnly(needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setGround(const bool needed, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setIntegrateGround(needed, file);
        calculators.at(id).second->setIntegrateGround(needed, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setArea(const bool needed, const qreal area, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setArea(needed, area, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::setRelativeCurv(const qreal a, const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).first->setRelativeCurv(a, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::update(const int file, const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
            calculators.at(id).first->update(file);
            calculators.at(id).second->update(file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}

void CalcWrapper::activateRef(const bool state, const int id, const int file){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ".  called");
    setOffset(true);
    if(id >= 0 && id < calculators.size()){
            calculators.at(id).second->activateRef(state, file);
    }else{
        log("Error: " + QString(this->metaObject()->className()) + "::" + __FUNCTION__  + ". id = " + QString::number(id));
    }
    setOffset();
}
