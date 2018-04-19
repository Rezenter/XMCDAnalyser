#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QScatterSeries>
#include <QChartView>
#include <QValueAxis>
#include <QCoreApplication>
#include <QSettings>
#include <QItemSelection>
#include <QThread>
#include <QFileSystemWatcher>
#include <QLinkedList>

#include "ui_mainwindow.h"
#include "extfsm.h"
#include "calcwrapper.h"
#include "calculator.h" //remove later
#include "pairwidget.h"
#include "logger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

signals:
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
    void setGround(const bool needed, const int file, const int id);
    void setRelativeCurv(const qreal a, const int file, const int id);
    void update(const int file, const int id);
    void appendCalc();
    void removeCalc(const int id);
    void log(QVariant);

private:
    QThread *calcThread;
    CalcWrapper *wrapper;
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
    void updateSummary();
    QtCharts::QChartView chartView;
    QtCharts::QChart chart;
    QtCharts::QChartView summaryChartView;
    QtCharts::QChart summaryChart;
    QtCharts::QLineSeries raw[2];
    QtCharts::QLineSeries norm[2];
    QtCharts::QLineSeries zero[2];
    QtCharts::QLineSeries xmcd;
    QtCharts::QLineSeries xmcdZero;
    QtCharts::QScatterSeries dot;
    QtCharts::QLineSeries line[2];
    QtCharts::QLineSeries steps;
    QtCharts::QValueAxis axisX;
    QtCharts::QValueAxis axisY;
    QtCharts::QValueAxis axisY2;
    QtCharts::QValueAxis summaryAxisX;
    QtCharts::QValueAxis summaryAxisY;
    ExtFSM *table = new ExtFSM(this);
    QString dataDir;
    QString filePath[2];
    QString fileName[2];
    qreal theta[2];
    qreal muB[4] = {9.274009994*pow(10,-24), 1, 5.7883818012*pow(10, -5), 9.274009994*pow(10, -21)};
    QString units[4] = {"J/T", "µβ", "eV/T", "erg/G"};
    QString sample = "not found";
    QString geom = "not found";
    QString energy = "not found";
    qreal angle[3] = {0.0, 55.0, 65.0};
    bool integrated[2];
    void rescale();
    QFileSystemWatcher *refresh;
    int id = -1;
    int file = 0;
    QList<PairWidget *> pairs;
    QList<QVector<QPair<qreal, QPointF>> const *> dataPointers[2];
    QList<QVector<QPair<qreal, QPointF>> const *> rawPointers[2];
    QList<QVector<QPair<qreal, QPointF>> const *> zeroPointers[2];
    QList<QVector<QPointF> const *> stepPointers[2];
    QList<QVector<QPointF> const *> xmcdPointers[2];
    QHash<QString, QVariant> defaults();
    void loadState(const QHash<QString, QVariant> state);
    void selectPair(const int id);
    int lastFile = 0;
    QLinkedList<QAbstractButton *> buttons;
    QLabel *summLabels[2];
    QLabel *dl2Labels[2];
    QLabel *dl3Labels[2];
    QLabel *mSELabels[2];
    QLabel *mOLabels[2];
    QLabel *phiLabels[2];
    QLabel *thetaLabels[2];
    QLabel *relationLabels[2];
    QLabel *fileNameLabel[2];
    QRadioButton *fileCheckBox[2];
    QCheckBox *holderBox[2];
    QDoubleSpinBox *offsets[2];
    Logger* logger;
    bool logEnabled = false;
    QString logPath = "";
    bool useExeLocation = true;
    bool debugMode = false;
    bool debugCopy = false;
    bool normalExit = false;
    qreal rawOffsetMult = 1E-11;
    qreal zeroOffsetMult = 1E-5;
    qreal normOffsetMult = 1E-6;
    qreal XMCDOffsetMult = 1E0;
    qreal stepSize = 10.0;

private slots:
    void open(QString);
    void exportDialog();
};

#endif // MAINWINDOW_H
