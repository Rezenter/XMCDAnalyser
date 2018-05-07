#ifndef CALCWARPER_H
#define CALCWARPER_H

#include <QDebug>//wtf i need this?

#include <QObject>

#include "calculator.h"
#include "refcalculator.h"

class CalcWrapper : public QObject{

    Q_OBJECT

public:
    CalcWrapper(QObject *parent = 0);
    ~CalcWrapper();
    void setRefPaths(QString commonPath);

signals:
    void dead();
    void processedData(const QVector<QPair<qreal, QPointF>>* points, const int id, const int file, const int ref);
    void rawData(const QVector<QPair<qreal, QPointF>>* points, const int id, const int file);
    void iZero(const QVector<QPair<qreal, QPointF>>* points, const int id, const int file);
    void stepData(const QVector<QPointF>* points, const int id, const int file);
    void XMCD(const QVector<QPointF>* points, const int id, const int file, const int ref);
    void integrals(const qreal* summ, const qreal* dl2, const qreal* dl3, const qreal* mSE,
                   const qreal* mO, const qreal* rel, const int id, const int file, const int ref);
    void moments(const qreal* mOP, const qreal* mOO, const qreal* ms, const qreal* mt, const int id, const int file);
    void linCoeffs(const QPointF* left, const QPointF* right, const QPointF* x, const int id, const int file);
    void completed(const int id, const int file, const int ref);
    void log(QVariant out);
    void setOffset(bool state = false);

public slots:
    void setLoader(const QString loaderPath, const int file, const int id, const QString refPath);
    void setLimits(const qreal left, const qreal right, const int file, const int id);
    void setEnergyShift(const qreal shift, const int file, const int id);
    void setShadowCurrent(const qreal signal, const qreal iZero, const int file, const int id);
    void setSmooth(const int count, const int file, const int id);
    void setDiff(const bool needed, const int file, const int id);
    void setLinearIntervals(const QPointF interval, const bool needed, const int file, const int id);
    void setNormalizationCoeff(const qreal coeff, bool needed, const int file, const int id, const int ref);
    void setStepped(const qreal coeff, const bool needed, const int file, const int id);
    void setIntegrate(const bool needed, const qreal index, const int file, const int id, const int ref);
    void setIntegrationConstants(const qreal newPc, const qreal newNh, const int id);
    void setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta, const int id);
    void setLin(const bool needed, const int file, const int id);
    void setPositiveIntegrals(const bool needed, const int file, const int id);
    void setGround(const bool needed, const int file, const int id);
    void setArea(const bool needed, const qreal area, const int file, const int id);
    void setRelativeCurv(const qreal a, const int file, const int id);
    void update(const int file, const int id);
    void appendCalc();
    void removeCalc(const int id);
    void activateRef(const bool state, const int id, const int file);

private:
    QList<QPair<QSharedPointer<Calculator>, QSharedPointer<RefCalculator>>> calculators;
    void switchConnection(const QPair<QSharedPointer<Calculator>, QSharedPointer<RefCalculator>> senderPair, const int newId);
    QString refCommonPath;
};

#endif // CALCWARPER_H

