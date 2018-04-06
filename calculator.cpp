#include "calculator.h"

Calculator::Calculator(QObject *parent) : QObject(parent){

}

/*to-do:
 * clean+optimize
 * implement box1
 * fix some actions availiable after last pair deleting
*/

Calculator::~Calculator(){
    emit dead();
}

void Calculator::setLoader(const QString loaderPath, const int file){
    if(file == 0 || file == 1){
        if(path[file] != loaderPath){
            path[file] = loaderPath;
            loaderChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setLimits(const qreal left, const qreal right, const int file){
    if(file == 0 || file == 1){
        if(tmpLimits[file] != QPointF(left, right)){
            tmpLimits[file] = QPointF(left, right);
            loaderChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setEnergyShift(const qreal shift, const int file){
    if(shift != tmpEnergyShift[file]){
        tmpEnergyShift[file] = shift;
        loaderChanged[file] = true;
        reset();
    }
}

void Calculator::setShadowCurrent(const qreal signal, const qreal iZero, const int file){
    if(file == 0 || file == 1){
        if(tmpShadow[file] != QPointF(signal, iZero)){
            tmpShadow[file] = QPointF(signal, iZero);
            shadowChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::load(const int file){
    if(file == 0 || file == 1){
        loader[file].setLimits(floor(limits[file].x()), floor(limits[file].y()), energyShift[file]);
        bare[file].resize(loader[file].getBareData().size());
        zero[file].resize(bare[file].size());
        for(int i = 0; i <  bare[file].size(); i++){
            QPair<qreal, QPair<qreal, qreal>> tmp = loader[file].getBareData().at(i);
            bare[file][i] = QPair< qreal, QPointF>(tmp.first, QPointF(tmp.second.first, tmp.second.second));
            tmp = loader[file].getZero().at(i);
            zero[file][i] = QPair< qreal, QPointF>(tmp.first, QPointF(tmp.second.first, tmp.second.second));
        }
        emit rawData(&bare[file]);
        emit iZero(&zero[file]);
        loaded[file] = true;
        calcData(file);
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::calcData(const int file){
    if(file == 0 || file == 1){
        if(loaded[file]){
            data[file].resize(bare[file].size());
            for(int i = 0; i <  bare[file].size(); i++){
                data[file][i] = QPair<qreal, QPointF>(bare[file][i].first,
                                                      QPointF((bare[file][i].second.x() - shadow[file].x()*1E-12)/(zero[file][i].second.x() - shadow[file].y()*1E-6),
                                                              (bare[file][i].second.y() - shadow[file].x()*1E-12)/(zero[file][i].second.y() - shadow[file].y()*1E-6)));
            }
            smooth(file);
        }else{
            qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". file  " << file << " not loaded.";
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setSmooth(const int count, const int file){
    if(file == 0 || file == 1){
        if(count >= 0 && count <= bare[file].size()){
            if(count != tmpSmoothPoints[file]){
                tmpSmoothPoints[file] = count;
                smoothChanged[file] = true;
                reset();
            }
        }else{
            qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable count == " << count;
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::smooth(const int file){
    if(file == 0 || file == 1){
        if(smoothPoints[file] > 1 && smoothPoints[file] <= data[file].size()){
            smoothedData[file].resize(data[file].size() - smoothPoints[file] + 1);
            qreal y[2] = {0.0};
            qreal x = 0.0;
            for(int i = 0; i < smoothPoints[file]; i++){
                x += data[file][i].first;
                y[0] += data[file][i].second.x();
                y[1] += data[file][i].second.y();
            }
            smoothedData[file][0] = QPair<qreal, QPointF>(x/smoothPoints[file], QPointF(y[0]/smoothPoints[file], y[1]/smoothPoints[file]));
            for(int i = smoothPoints[file]; i < data[file].size(); i++){
                x += data[file][i].first;
                x -= data[file][i - smoothPoints[file]].first;
                y[0] += data[file][i].second.x();
                y[1] += data[file][i].second.y();
                y[0] -= data[file][i - smoothPoints[file]].second.x();
                y[1] -= data[file][i - smoothPoints[file]].second.y();
                smoothedData[file][i - smoothPoints[file] + 1] = QPair<qreal, QPointF>(x/smoothPoints[file], QPointF(y[0]/smoothPoints[file], y[1]/smoothPoints[file]));
            }
        }else if(smoothPoints[file] == 1){
            smoothedData[file].resize(data[file].size());
            smoothedData[file] = data[file];
        }
        if(normalizationNeeded[file]){
            normalize(file);
        }else if(diffNeeded[file]){
            normData[file].resize(smoothedData[file].size());
            normData[file] = smoothedData[file];
            calcDiff(file);
        }else{
            emit processedData(&smoothedData[file]);
            ready = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setNormalizationCoeff(const qreal coeff, bool needed, const int file){
    if(file == 0 || file == 1){
        if(tmpNormalizationCoeff[file][0] != coeff){
            tmpNormalizationCoeff[file][0] = coeff;
            normalizationChanged[file] = true;
        }
        if(needed != tmpNormalizationNeeded[file]){
            tmpNormalizationNeeded[file] = needed;
            normalizationChanged[file] = true;
        }
        if(normalizationChanged[file]){
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setRelativeCurv(const qreal a, const int file){
    if(file == 0 || file == 1){
        if(a != tmpRelativeCurv[file]){
            tmpRelativeCurv[file] = a;
            normalizationChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::normalize(const int file){
    if(file == 0 || file == 1){
        normData[file].resize(smoothedData[file].size());
        qreal x1 = smoothedData[file].first().first;
        qreal x2 = smoothedData[file].last().first;
        qreal y1 = smoothedData[file].first().second.x()/smoothedData[file].first().second.y();
        qreal y2 = smoothedData[file].last().second.x()/smoothedData[file].last().second.y();
        //insert relative curv here
        normalizationCoeff[file][1] = (normalizationCoeff[file][0]*(qPow(x1, 2.0) - qPow(x2, 2.0)))/(x2 - x1);
        normalizationCoeff[file][2] = 1 - normalizationCoeff[file][1]*x1 - normalizationCoeff[file][0]*qPow(x1, 2.0);
        qreal relB = (y2 - y1 + relativeCurv[file]*(qPow(x1, 2.0) - qPow(x2, 2.0)))/(x2 - x1);
        qreal relC = y1 - relB*x1 - relativeCurv[file]*qPow(x1, 2.0);
        for(int i = 0; i < smoothedData[file].size(); i++){
            normData[file][i].first = smoothedData[file][i].first;
            normData[file][i].second.rx() = (smoothedData[file][i].second.x())*
                    (normalizationCoeff[file][0]*qPow(smoothedData[file][i].first, 2.0) +
                    normalizationCoeff[file][1]*smoothedData[file][i].first + normalizationCoeff[file][2]);
            normData[file][i].second.ry() = (smoothedData[file][i].second.y()*
                                             (relativeCurv[file]*qPow(smoothedData[file][i].first, 2.0) +
                                              relB*smoothedData[file][i].first + relC))*
                    (normalizationCoeff[file][0]*qPow(smoothedData[file][i].first, 2.0) +
                    normalizationCoeff[file][1]*smoothedData[file][i].first + normalizationCoeff[file][2]);
        }
        if(diffNeeded[file]){
            calcDiff(file);
        }else{
            emit processedData(&normData[file]);
            ready = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setDiff(const bool needed, const int file){
    if(file == 0 || file == 1){
        if(tmpDiffNeeded[file] != needed){
            tmpDiffNeeded[file] = needed;
            diffChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::calcDiff(const int file){
    if(file == 0 || file == 1){
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
        emit XMCD(&diff[file]);//edit after box1
        if(linearNeeded[file]){
            linear(file);
        }else{
            emit processedData(&normData[file]);
            ready = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setLinearIntervals(const QPointF interval, bool needed, const int file){
    if(file == 0 || file == 1){
        if(tmpLinearIntervals[file] != interval){
            tmpLinearIntervals[file] = interval;
            linearIntervalsChanged[file] = true;
        }
        if(needed != tmpLinearNeeded[file]){
            tmpLinearNeeded[file] = needed;
            linearIntervalsChanged[file] = true;
        }
        if(linearIntervalsChanged[file]){
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::linear(const int file){
    if(linearIntervals[file].x() < normData[file].size() && linearIntervals[file].x() > 1){
        if(linearIntervals[file].y() < normData[file].size() && linearIntervals[file].y() > 1){
            qreal xSum[2] = {0.0, 0.0};
            qreal ySum[2] = {0.0, 0.0};
            qreal x2Sum[2] = {0.0, 0.0};
            qreal xySum[2] = {0.0, 0.0};
            int integer[2] = {qFloor(linearIntervals[file].x()), qFloor(linearIntervals[file].y())};
            qreal rest[2] = {(linearIntervals[file].x() - integer[0]), 1 - (linearIntervals[file].y() - integer[1])};
            qreal x[2] = {normData[file][integer[0]].first, normData[file][normData[file].size() - integer[1]].first};
            qreal y[2];
            for(int i = 0; i < integer[0]; i++){
                xSum[0] += normData[file][i].first;
                x2Sum[0] += qPow(normData[file][i].first, 2.0);
                ySum[0] += (normData[file][i].second.x() + normData[file][i].second.y())/2.0;
                xySum[0] += normData[file][i].first*(normData[file][i].second.x() + normData[file][i].second.y())/2.0;
            }
            if((rest[0] >= 0.01 && rest[0] <= 0.99)){
                x[0] = normData[file][integer[0] - 1].first +
                        (normData[file][integer[0]].first - normData[file][integer[0] - 1].first)*rest[0];
                y[0] = (normData[file][integer[0] - 1].second.x() +
                        (normData[file][integer[0]].second.x() - normData[file][integer[0] - 1].second.x())*rest[0] +
                        normData[file][integer[0] - 1].second.y() +
                        (normData[file][integer[0]].second.y() - normData[file][integer[0] - 1].second.y())*rest[0])/2.0;
                xSum[0] += x[0];
                x2Sum[0] += qPow(x[0], 2.0);
                ySum[0] += y[0];
                xySum[0] += x[0]*y[0];
                integer[0] += 1;
            }
            for(int i = normData[file].size() - 1; i > normData[file].size() - 1 - integer[1]; i--){
                xSum[1] += normData[file][i].first;
                x2Sum[1] += qPow(normData[file][i].first, 2.0);
                ySum[1] += (normData[file][i].second.x() + normData[file][i].second.y())/2;
                xySum[1] += normData[file][i].first*(normData[file][i].second.x() + normData[file][i].second.y())/2;
            }

            if(rest[1] >= 0.01 && rest[1] <= 0.99){
                x[1] = normData[file][normData[file].size() - integer[1] - 1].first +
                        (normData[file][normData[file].size() - integer[1]].first - normData[file][normData[file].size() - integer[1] - 1].first)*rest[1];
                y[1] = (normData[file][normData[file].size() - integer[1] - 1].second.x() +
                        (normData[file][normData[file].size() - integer[1]].second.x() -
                        normData[file][normData[file].size() - integer[1] - 1].second.x())*rest[1] +
                        normData[file][normData[file].size() - integer[1] - 1].second.y() +
                        (normData[file][normData[file].size() - integer[1]].second.y() -
                        normData[file][normData[file].size() - integer[1] - 1].second.y())*rest[1])/2.0;
                xSum[1] += x[1];
                x2Sum[1] += qPow(x[1], 2.0);
                ySum[1] += y[1];
                xySum[1] += x[1]*y[1];
                integer[1] += 1;
            }
            for(int i = 0; i < 2; i++){
                linearCoeff[file][i].rx() = (integer[i]*xySum[i] - xSum[i]*ySum[i])/(integer[i]*x2Sum[i] - pow(xSum[i], 2));
                linearCoeff[file][i].ry() = (ySum[i] - linearCoeff[file][i].rx()*xSum[i])/integer[i];
            }
            xPoint[file] = QPointF(x[0], x[1]);
            emit linCoeffs(&linearCoeff[file][0], &linearCoeff[file][1], &xPoint[file]);
            linData[file].resize(normData[file].size());
            for(int i = 0; i < normData[file].size(); i++){
                qreal x = normData[file][i].first;
                qreal linearBackground = linearCoeff[file][0].x()*x + linearCoeff[file][0].y();
                qreal y1 = normData[file][i].second.x() - linearBackground;
                qreal y2 = normData[file][i].second.y() - linearBackground;
                linData[file][i] = QPair<qreal, QPointF>(x, QPointF(y1, y2));
            }
            if(linNeeded[file]){
                emit processedData(&linData[file]);
            }else{
                emit processedData(&normData[file]);
            }
            fitData[file].resize(linData[file].size()); //
            fitData[file] = linData[file];              //remove after box1 patch
            finalDiff[file].resize(diff[file].size());  //
            finalDiff[file] = diff[file];
            calcSteps(file);
        }else{
            qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable linearIntervals[file].y() == " << linearIntervals[file].y();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable linearIntervals[file].x() == " << linearIntervals[file].x();
    }
}

void Calculator::calcSteps(const int file){
    if(file == 0 || file == 1){
        steps[file].resize(fitData[file].size());
        qreal m = ((fitData[file].last().second.x() - fitData[file][0].second.x()) +
                    (fitData[file].last().second.y() - fitData[file][0].second.y()))/(6 * M_PI);
        for(int i = 0; i < fitData[file].size(); i++){
            qreal x = fitData[file][i].first;
            steps[file][i].setX(fitData[file][i].first);
            steps[file][i].setY((fitData[file][0].second.x() + fitData[file][0].second.y())/2.0 +
                    m * (M_PI_2 + qAtan(steppedCoeff[file]*(x - fitData[file][lEdges[file]].first))) * 2 +
                    m * (M_PI_2 + qAtan(steppedCoeff[file]*(x - fitData[file][rEdges[file]].first))));
        }
        emit stepData(&steps[file]);
        if(stepFitNeeded[file] && linNeeded){
            //box1
        }else if(steppedNeeded[file] && linNeeded){
            stepped(file);
        }else{
            ready = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setLin(const bool needed, const int file){
    if(file == 0 || file == 1){
        if(needed != tmpLinNeeded[file]){
            tmpLinNeeded[file] = needed;
            linChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setStepped(const qreal coeff, const bool needed, const int file){
    if(file == 0 || file == 1){
        if(tmpSteppedCoeff[file] != coeff){
            tmpSteppedCoeff[file] = coeff;
            steppedCoeffChanged[file] = true;
        }
        if(needed != tmpSteppedNeeded[file]){
            tmpSteppedNeeded[file] = needed;
            steppedCoeffChanged[file] = true;
        }
        if(steppedCoeffChanged[file]){
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::stepped(const int file){
    if(file == 0 || file == 1){
        finalData[file].resize(fitData[file].size());
        for(int i = 0; i < fitData[file].size(); i++){
            qreal x = fitData[file][i].first;
            qreal y1 = fitData[file][i].second.x() - steps[file][i].y();
            qreal y2 = fitData[file][i].second.y() - steps[file][i].y();
            finalData[file][i] = QPair<qreal, QPointF>(x, QPointF(y1, y2));
        }
        emit processedData(&finalData[file]);
        if(integrateNeeded[file]){
            integrate(file);
        }else{
            ready = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setIntegrate(const bool needed, const int index, const int file){
    if(file == 0 || file == 1){
        if(index > lEdges[file] && index < rEdges[file]){
            tmpSeparator[file] = index;
            tmpIntegrateNeeded[file] = needed;
            integrationChanged[file] = true;
            reset();
        }else{
            qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable index == " << index;
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setIntegrationConstants(const qreal newPc, const qreal newNh){
    if(tmpConstant != newNh/newPc){
        if(newPc > 0.0 && newPc <= 1.0){
            tmpConstant = newNh/newPc;
            constantsChanged = true;
            reset();
        }else{
           qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable newPc == " << newPc;
        }
    }
}

void Calculator::setIntegratePositiveOnly(const bool needed, const int file){
    if(file == 0 || file == 1){
        if(needed != tmpIntegratePositiveOnly[file]){
            tmpIntegratePositiveOnly[file] = needed;
            integrationChanged[file] = true;
            reset();
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::integrate(const int file){
    if(file == 0 || file == 1){
        if(steppedNeeded[file]){
            if(diffNeeded[file]){
                summInt[file] = 0.0;
                for(int i = 0; i < finalData[file].size() - 1; i++){
                    summInt[file] += (finalData[file][i + 1].first - finalData[file][i].first)*
                            (finalData[file][i + 1].second.x() + finalData[file][i].second.x() +
                             finalData[file][i + 1].second.y() + finalData[file][i].second.y())/2.0;
                }
                qreal a = (finalDiff[file][0].y() - finalDiff[file][separator[file]].y())/(finalDiff[file][0].x() - finalDiff[file][separator[file]].x());
                qreal b = finalDiff[file][0].y() - a*finalDiff[file][0].x();
                dl3Int[file] = 0.0;
                for(int i = 0; i < separator[file]; i++){
                    qreal trap = ((finalDiff[file][i+1].x() - finalDiff[file][i].x())*(finalDiff[file][i+1].y() + finalDiff[file][i].y())/2.0) -
                            (finalDiff[file][i+1].x() - finalDiff[file][i].x())*(a*(finalDiff[file][i+1].x() + finalDiff[file][i].x())/2.0 + b);
                    if(!integratePositiveOnly[file] || trap < 0.0){
                        dl3Int[file] += trap;
                    }
                }
                a = (finalDiff[file][separator[file]].y() - finalDiff[file][finalData[file].size() - 1].y())/
                        (finalDiff[file][separator[file]].x() - finalDiff[file][finalData[file].size() - 1].x());
                b = finalDiff[file][separator[file]].y() - a*finalDiff[file][separator[file]].x();
                dl2Int[file] = 0.0;
                for(int i = separator[file]; i < finalData[file].size() - 1; i++){
                    qreal trap = ((finalDiff[file][i+1].x() - finalDiff[file][i].x())*(finalDiff[file][i+1].y() + finalDiff[file][i].y())/2.0) -
                            (finalDiff[file][i+1].x() - finalDiff[file][i].x())*(a*(finalDiff[file][i+1].x() + finalDiff[file][i].x())/2.0 + b);
                    if(!integratePositiveOnly[file] || trap > 0.0){
                        dl2Int[file] += trap;
                    }
                }
                mSEff[file] = -2.0*constant*(dl3Int[file] - 2.0*dl2Int[file])/summInt[file];
                mOrb[file] = -(4.0/3.0)*constant*(dl3Int[file] + dl2Int[file])/summInt[file];
                relation[file] = 1.5*(dl3Int[file] - 2.0*dl2Int[file])/(dl3Int[file] + dl2Int[file]);
                emit integrals(&summInt[file], &dl2Int[file], &dl3Int[file], &mSEff[file], &mOrb[file], &relation[file]);
                if(calculateNeeded){
                    calculate();
                }else{
                    ready = true;
                    reset();
                }
            }else{
                qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable diffNeeded == " << diffNeeded[file];
            }
        }else{
            qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable steppedNeeded == " << steppedNeeded[file];
        }
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable file == " << file;
    }
}

void Calculator::setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta){
    if(tmpCalculateNeeded != needed){
        tmpCalculateNeeded = needed;
        calculateChanged = true;
    }
    if(tmpPhi != newPhi){
        tmpPhi = newPhi;
        calculateChanged = true;
    }
    if(tmpTheta != newTheta){
        tmpTheta = newTheta;
        calculateChanged = true;
    }
    if(calculateChanged){
        reset();
    }
}

void Calculator::calculate(){
    if(integrateNeeded[0] && integrateNeeded[1]){
        mOrbO = -(mOrb[0]*qSin(theta.y())*qSin(phi.y()) - qSin(theta.x())*qSin(phi.x())*mOrb[1])/
                (qSin(theta.x())*qSin(phi.x())*qCos(theta.y())*qCos(phi.y()) - qSin(theta.y())*qSin(phi.y())*qCos(theta.x())*qCos(phi.x()));
        mOrbP = (-mOrb[1]*qCos(theta.x())*qCos(phi.x()) + mOrb[0]*qCos(theta.y())*qCos(phi.y()))/
                (qSin(theta.x())*qSin(phi.x())*qCos(theta.y())*qCos(phi.y()) - qSin(theta.y())*qSin(phi.y())*qCos(theta.x())*qCos(phi.x()));
        mS = (2*qCos(theta.x())*qCos(phi.x())*mSEff[1] - qSin(theta.x())*qSin(phi.x())*mSEff[1] -
                2*mSEff[0]*qCos(theta.y())*qCos(phi.y()) + mSEff[0]*qSin(theta.y())*qSin(phi.y()))/
                (2*qCos(phi.y() - theta.y())*qCos(theta.x())*qCos(phi.x()) - qCos(phi.y() - theta.y())*qSin(theta.x())*qSin(phi.x()) -
                2*qCos(theta.y())*qCos(phi.y())*qCos(phi.x() - theta.x()) + qSin(theta.y())*qSin(phi.y())*qCos(phi.x()- theta.x()));
        mT = -0.2857142857*(mSEff[1]*qCos(phi.x() - theta.x()) - mSEff[0]*qCos(phi.y() - theta.y()))/
                (2*qCos(phi.y() - theta.y())*qCos(theta.x())*qCos(phi.x()) - qCos(phi.y() - theta.y())*qSin(theta.x())*qSin(phi.x()) -
                2*qCos(theta.y())*qCos(phi.y())*qCos(phi.x() - theta.x()) + qSin(theta.y())*qSin(phi.y())*qCos(phi.x()- theta.x()));
        emit moments(&mOrbO, &mOrbP, &mS, &mT);
        ready = true;
        reset();
    }else{
        qDebug() << "error in " << this->metaObject()->className() << "::" << __FUNCTION__  << ". Variable integrateNeeded == " << integrateNeeded[0] << integrateNeeded[1];
    }
}

void Calculator::reset(){
    if(ready){
        for(int file = 0; file < 2; file++){
            if(loaderChanged[file]){
                loaderChanged[file] = false;
                loader[file] = FileLoader(path[file]);
                limits[file] = tmpLimits[file];
                energyShift[file] = tmpEnergyShift[file];
                loaded[file] = false;
                load(file);
            }else if(loaded[file]){
                if(shadowChanged[file]){
                    shadowChanged[file] = false;
                    shadow[file] = tmpShadow[file];
                    calcData(file);
                }else if(smoothChanged[file]){
                    smoothChanged[file] = false;
                    smoothPoints[file] = tmpSmoothPoints[file];
                    smooth(file);
                }else if(normalizationChanged[file]){
                    normalizationChanged[file] = false;
                    normalizationCoeff[file][0] = tmpNormalizationCoeff[file][0]*1E-4;
                    relativeCurv[file] = tmpRelativeCurv[file]*1E-4;
                    normalizationNeeded[file] = tmpNormalizationNeeded[file];
                    if(normalizationNeeded[file]){
                        normalize(file);
                    }else{
                        smooth(file);
                    }
                }else if(diffChanged[file]){
                    diffChanged[file] = false;
                    diffNeeded[file] = tmpDiffNeeded[file];
                    if(diffNeeded[file]){
                        calcDiff(file);
                    }
                }else if(linearIntervalsChanged[file]){
                    linearIntervalsChanged[file] = false;
                    linearIntervals[file] = tmpLinearIntervals[file];
                    linearNeeded[file] = tmpLinearNeeded[file];
                    if(linearNeeded[file]){
                        linear(file);
                    }
                }else if(linChanged[file]){
                    linChanged[file] = false;
                    linNeeded[file] = tmpLinNeeded[file];
                    linear(file);
                }else if(steppedCoeffChanged[file]){ //box 1 before
                    steppedCoeffChanged[file] = false;
                    steppedCoeff[file] = tmpSteppedCoeff[file];
                    steppedNeeded[file] = tmpSteppedNeeded[file];
                    if(steppedNeeded[file]){
                        calcSteps(file);
                    }else{
                        linear(file);
                    }
                }else if(integrationChanged[file]){
                    integrationChanged[file] = false;
                    separator[file] = tmpSeparator[file];
                    integrateNeeded[file] = tmpIntegrateNeeded[file];
                    integratePositiveOnly[file] = tmpIntegratePositiveOnly[file];
                    if(integrateNeeded[file]){
                        integrate(file);
                    }
                }else if(constantsChanged){
                    constantsChanged = false;
                    constant = tmpConstant;
                    if(loaded[0] && integrateNeeded[0]){
                        integrate(0);
                    }
                    if(loaded[1] && integrateNeeded[1]){
                        integrate(1);
                    }
                }else if(calculateChanged){
                    calculateChanged = false;
                    phi = tmpPhi;
                    theta = tmpTheta;
                    calculateNeeded = tmpCalculateNeeded;
                    if(loaded[0] && loaded[1] && calculateNeeded){
                        calculate();
                    }else{
                        qDebug() << "ui error at calcChanged";
                    }
                }
            }
            emit completed();
        }
    }
}

void Calculator::update(const int file){
    loaderChanged[file] = true;
    reset();
}
