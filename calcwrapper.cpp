#include "calcwrapper.h"

CalcWrapper::CalcWrapper(QObject *parent) : QObject(parent){

}

/*to-do:

*/

CalcWrapper::~CalcWrapper(){
    emit dead();
}



void CalcWrapper::appendCalc(){
    QSharedPointer<Calculator> p (new Calculator());
    //assume corresponding metatypes are already registered in mainwindow.cpp
    establishConnections(p, calculators.size());
    calculators.append(p);

}

void CalcWrapper::establishConnections(const QSharedPointer<Calculator> sender, const int id){
    QObject::connect(sender.data(), &Calculator::processedData, this, [=](const QVector<QPair<qreal, QPointF>>* points){
        emit processedData(points, id);
    });
    QObject::connect(sender.data(), &Calculator::rawData, this, [=](const QVector<QPair<qreal, QPointF>>* points){
        emit rawData(points, id);
    });
    QObject::connect(sender.data(), &Calculator::iZero, this, [=](const QVector<QPair<qreal, QPointF>>* points){
        emit iZero(points, id);
    });
    QObject::connect(sender.data(), &Calculator::stepData, this, [=](const QVector<QPointF>* points){
        emit stepData(points, id);
    });
    QObject::connect(sender.data(), &Calculator::XMCD, this, [=](const QVector<QPointF>* points){
        emit XMCD(points, id);
    });
    QObject::connect(sender.data(), &Calculator::integrals, this, [=](const qreal* summ, const qreal* dl2, const qreal* dl3,
                     const qreal* mSE, const qreal* mO, const qreal* rel){
        emit integrals(summ, dl2, dl3, mSE,  mO, rel, id);
    });
    QObject::connect(sender.data(), &Calculator::moments, this, [=](const qreal* mOP, const qreal* mOO, const qreal* ms, const qreal* mt){
        emit moments(mOP, mOO, ms, mt, id);
    });
    QObject::connect(sender.data(), &Calculator::linCoeffs, this, [=](const QPointF* left, const QPointF* right, const QPointF* x){
        emit linCoeffs(left, right, x, id);
    });
    QObject::connect(sender.data(), &Calculator::completed, this, [=]{
        emit completed(id);
    });
}

void CalcWrapper::removeCalc(const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.removeAt(id);
        for(int i = id; i < calculators.size(); ++i){
            calculators.at(i).data()->disconnect();
            establishConnections(calculators.at(i), id);
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}

void CalcWrapper::setLoader(const QString loaderPath, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setLoader(loaderPath, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setLimits(const qreal left, const qreal right, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setLimits(left, right, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setEnergyShift(const qreal shift, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setEnergyShift(shift, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setShadowCurrent(const qreal signal, const qreal iZero, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setShadowCurrent(signal, iZero, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setSmooth(const int count, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setSmooth(count, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setDiff(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setDiff(needed, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setLinearIntervals(const QPointF interval, const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setLinearIntervals(interval, needed, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setNormalizationCoeff(const qreal coeff, bool needed, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setNormalizationCoeff(coeff, needed, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setStepped(const qreal coeff, const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setStepped(coeff, needed, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setIntegrate(const bool needed, const int index, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setIntegrate(needed, index, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setIntegrationConstants(const qreal newPc, const qreal newNh, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setIntegrationConstants(newPc, newNh);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setCalculate(needed, newPhi, newTheta);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setLin(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setLin(needed, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setPositiveIntegrals(const bool needed, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setIntegratePositiveOnly(needed, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::setRelativeCurv(const qreal a, const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->setRelativeCurv(a, file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
void CalcWrapper::update(const int file, const int id){
    if(id >= 0 && id < calculators.size()){
        calculators.at(id).data()->update(file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable id == " << id;
    }
}
