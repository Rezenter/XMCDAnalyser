#ifndef CALCWARPER_H
#define CALCWARPER_H

#include <QDebug>

#include <QObject>

#include "calculator.h"

class CalcWrapper : public QObject{

    Q_OBJECT

public:
    CalcWrapper(QObject *parent = 0);
    ~CalcWrapper();

signals:
    void dead();
    void processedData(const QVector<QPair<qreal, QPointF>>* points, const int id);
    void rawData(const QVector<QPair<qreal, QPointF>>* points, const int id);
    void iZero(const QVector<QPair<qreal, QPointF>>* points, const int id);
    void stepData(const QVector<QPointF>* points, const int id);
    void XMCD(const QVector<QPointF>* points, const int id);
    void integrals(const qreal* summ, const qreal* dl2, const qreal* dl3, const qreal* mSE, const qreal* mO, const qreal* rel, const int id);
    void moments(const qreal* mOP, const qreal* mOO, const qreal* ms, const qreal* mt, const int id);
    void linCoeffs(const QPointF* left, const QPointF* right, const QPointF* x, const int id);
    void completed(const int id);

public slots:
    void setLoader(const QString loaderPath, const int file, const int id);
    void setLimits(const qreal left, const qreal right, const int file, const int id);
    void setEnergyShift(const qreal shift, const int file, const int id);
    void setShadowCurrent(const qreal signal, const qreal iZero, const int file, const int id);
    void setSmooth(const int count, const int file, const int id);
    void setDiff(const bool needed, const int file, const int id);
    void setLinearIntervals(const QPointF interval, const bool needed, const int file, const int id);
    void setNormalizationCoeff(const qreal coeff, bool needed, const int file, const int id);
    void setStepped(const qreal coeff, const bool needed, const int file, const int id);
    void setIntegrate(const bool needed, const int index, const int file, const int id);
    void setIntegrationConstants(const qreal newPc, const qreal newNh, const int id);
    void setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta, const int id);
    void setLin(const bool needed, const int file, const int id);
    void setPositiveIntegrals(const bool needed, const int file, const int id);
    void setRelativeCurv(const qreal a, const int file, const int id);
    void update(const int file, const int id);
    void appendCalc();//connect newCalc to enhanced signals
    void removeCalc(const int id);

private:
    QList<QSharedPointer<Calculator>> calculators;
    void establishConnections(const QSharedPointer<Calculator> sender, const int id);
};

#endif // CALCWARPER_H

