#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QDebug>

#include <QObject>
#include "fileloader.h"
#include <QPointF>
#include <QtMath>
#include <QDateTime>

class Calculator : public QObject{

    Q_OBJECT

public:
    Calculator(QObject *parent = 0);
    ~Calculator();

signals:
    void dead();
    void processedData(const QVector<QPair<qreal, QPointF>>* points);
    void rawData(const QVector<QPair<qreal, QPointF>>* points);
    void iZero(const QVector<QPair<qreal, QPointF>>* points);
    void stepData(const QVector<QPointF>* points);
    void XMCD(const QVector<QPointF>* points);
    void integrals(const qreal* summ, const qreal* dl2, const qreal* dl3, const qreal* mSE, const qreal* mO, const qreal* rel);
    void moments(const qreal* mOP, const qreal* mOO, const qreal* ms, const qreal* mt);
    void linCoeffs(const QPointF* left, const QPointF* right, const QPointF* x);
    void completed();

public slots:
    void setLoader(const QString loaderPath, const int file);
    void setLimits(const qreal left, const qreal right, const int file);
    void setEnergyShift(const qreal shift, const int file);
    void setShadowCurrent(const qreal signal, const qreal iZero, const int file);
    void setSmooth(const int count, const int file);
    void setDiff(const bool needed, const int file);//unused
    void setLinearIntervals(const QPointF interval, bool needed, const int file);
    void setNormalizationCoeff(const qreal coeff, bool needed, const int file);
    void setRelativeCurv(const qreal a, const int file);
    void setLin(const bool needed, const int file);
    void setStepped(const qreal coeff, bool needed, const int file);
    void setIntegrate(const bool needed, const int index, const int file);
    void setIntegrationConstants(const qreal newPc, const qreal newNh);
    void setIntegratePositiveOnly(const bool needed, const int file);
    void setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta);
    void update(const int file);

private:
    QVector<QPair<qreal, QPointF>> data[2];
    QVector<QPair<qreal, QPointF>> bare[2];
    QVector<QPair<qreal, QPointF>> zero[2];
    QVector<QPair<qreal, QPointF>> smoothedData[2];
    QVector<QPair<qreal, QPointF>> normData[2];
    QVector<QPair<qreal, QPointF>> linData[2];
    QVector<QPair<qreal, QPointF>> fitData[2];
    QVector<QPair<qreal, QPointF>> finalData[2];
    QVector<QPointF> steps[2];
    QVector<QPointF> diff[2];
    QVector<QPointF> finalDiff[2];
    FileLoader loader[2];
    QString path[2] = {"", ""};
    bool loaderChanged[2] = {false, false};
    bool loaded[2] = {false, false};
    bool shadowChanged[2] = {false, false};
    bool smoothChanged[2] = {false, false};
    bool normalizationChanged[2] = {false, false};
    bool normalizationNeeded[2] = {false, false};
    bool tmpNormalizationNeeded[2] = {false, false};
    bool linearIntervalsChanged[2] = {false, false};
    bool linearNeeded[2] = {false, false};
    bool tmpLinearNeeded[2] = {false, false};
    bool linChanged[2] = {false, false};
    bool linNeeded[2] = {false, false};
    bool tmpLinNeeded[2] = {false, false};
    bool diffNeeded[2] = {true, true};
    bool tmpDiffNeeded[2] = {true, true};
    bool diffChanged[2] = {false, false};
    bool steppedNeeded[2] = {false, false};
    bool tmpSteppedNeeded[2] = {false, false};
    bool steppedCoeffChanged[2] = {false, false};
    bool integrationChanged[2] = {false, false};
    bool integrateNeeded[2] = {false, false};
    bool tmpIntegrateNeeded[2] = {false, false};
    bool integratePositiveOnly[2] = {false, false};
    bool tmpIntegratePositiveOnly[2] = {false, false};
    bool stepFitNeeded[2] = {false, false};//currently constantly false
    bool ready = true;
    bool constantsChanged = false;
    bool calculateNeeded = false;
    bool calculateChanged = false;
    bool tmpCalculateNeeded = false;
    QPointF linearCoeff[2][2]; //[file][left, right](a, b)
    QPointF limits[2] = {QPointF(0, 0), QPointF(0.0 , 0.0)};
    QPointF tmpLimits[2] = {QPointF(0, 0), QPointF(0.0 , 0.0)};
    QPointF shadow[2] = {QPointF(0.0, 0.0), QPointF(0.0 , 0.0)};
    QPointF tmpShadow[2] = {QPointF(0.0, 0.0), QPointF(0.0 , 0.0)};
    QPointF max[2] = {QPointF(0.0, 0.0), QPointF(0.0 , 0.0)};
    QPointF linearIntervals[2] = {QPointF(2.0, 2.0), QPointF(2.0 , 2.0)};
    QPointF tmpLinearIntervals[2] = {QPointF(2.0, 2.0), QPointF(2.0 , 2.0)};
    QPointF phi = QPointF(0.0 , 0.0);
    QPointF tmpPhi = QPointF(0.0 , 0.0);
    QPointF theta = QPointF(0.0 , 0.0);
    QPointF tmpTheta = QPointF(0.0 , 0.0);
    QPointF xPoint[2] = {QPointF(0, 0), QPointF(0.0 , 0.0)};
    qreal energyShift[2] = {0.0, 0.0};
    qreal tmpEnergyShift[2] = {0.0, 0.0};
    qreal normalizationCoeff[2][3]; //[file][a, b, c]
    qreal tmpNormalizationCoeff[2][3];
    qreal relativeCurv[2];
    qreal tmpRelativeCurv[2];
    qreal steppedCoeff[2] = {1.0, 1.0};
    qreal tmpSteppedCoeff[2] = {1.0, 1.0};
    qreal summInt[2] = {0.0, 0.0};
    qreal dl2Int[2] = {0.0, 0.0};
    qreal dl3Int[2] = {0.0, 0.0};
    qreal mSEff[2] = {0.0, 0.0};
    qreal mOrb[2] = {0.0, 0.0};
    qreal relation[2] = {0.0, 0.0};
    qreal mOrbP = 0.0;
    qreal mOrbO = 0.0;
    qreal mS = 0.0;
    qreal mT = 0.0;
    qreal constant;
    qreal tmpConstant;
    void load(const int file);
    void calcData(const int file);
    void smooth(const int file);
    void normalize(const int file);
    void linear(const int file);
    void calcSteps(const int file);
    void calcDiff(const int file);
    void stepped(const int file);
    void integrate(const int file);
    void calculate();
    void reset();
    int smoothPoints[2] = {1, 1};
    int tmpSmoothPoints[2] = {1, 1};
    int lEdges[2] = {0, 0};
    int rEdges[2] = {0, 0};
    int separator[2];
    int tmpSeparator[2];
};

#endif // Calculator_H
