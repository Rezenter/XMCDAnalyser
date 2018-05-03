#ifndef RefCalculator_H
#define RefCalculator_H

#include <QObject>
#include <QPointF>
#include <QtMath>
#include <QDateTime>

#include "fileloader.h"

class RefCalculator : public QObject{

    Q_OBJECT

public:
    RefCalculator(QObject *parent = 0);
    ~RefCalculator();

signals:
    void processedData(const QVector<QPair<qreal, QPointF>>* points, const int file);
    void XMCD(const QVector<QPointF>* points, const int file);
    void integrals(const qreal* summ, const qreal* dl2, const qreal* dl3, const int file);
    void completed(const int file);
    void log(QVariant out);
    void setOffset(bool state = false);

public slots:
    void setLoader(const QString loaderPath, const int file);
    void setEnergyShift(const qreal shift, const int file);
    void setNormalizationCoeff(const qreal coeff, const int file);
    void setStepped(const qreal coeff, const int file);
    void setIntegrate(const qreal separatorValue, const int file);
    void setIntegratePositiveOnly(const bool needed, const int file);
    void setIntegrateGround(const bool needed, const int file);
    void update(const int file);
    void activateRef(const bool state, const int file);

private:
    QVector<QPair<qreal, QPointF>> data[2];
    QVector<QPair<qreal, QPointF>> normData[2];
    QVector<QPair<qreal, QPointF>> finalData[2];
    QVector<QPointF> diff[2];
    FileLoader loader[2];
    QString path[2] = {"", ""};
    bool loaderChanged[2] = {false, false};
    bool loaded[2] = {false, false};
    bool normalizationChanged[2] = {false, false};
    bool steppedCoeffChanged[2] = {false, false};
    bool integrationChanged[2] = {false, false};
    bool integratePositiveOnly[2] = {false, false};
    bool tmpIntegratePositiveOnly[2] = {false, false};
    bool integrateGround[2] = {false, false};
    bool tmpIntegrateGround[2] = {false, false};
    bool ready = true;
    bool active[2] = {false, false};
    bool tmpActive[2] = {false, false};
    qreal energyShift[2] = {0.0, 0.0};
    qreal tmpEnergyShift[2] = {0.0, 0.0};
    qreal normalizationCoeff[2][3]; //[file][a, b, c]
    qreal tmpNormalizationCoeff[2][3];
    qreal steppedCoeff[2] = {1.0, 1.0};
    qreal tmpSteppedCoeff[2] = {1.0, 1.0};
    qreal summInt[2] = {0.0, 0.0};
    qreal dl2Int[2] = {0.0, 0.0};
    qreal dl3Int[2] = {0.0, 0.0};
    qreal separator[2] = {0.0, 0.0};
    qreal tmpSeparator[2] = {0.0, 0.0};
    void load(const int file);
    void normalize(const int file);
    void stepped(const int file);
    void integrate(const int file);
    void reset();
    int lEdges[2] = {0, 0};
    int rEdges[2] = {0, 0};
};

#endif // RefCalculator_H
