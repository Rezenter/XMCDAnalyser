#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>
#include <QCoreApplication>
#include <QSettings>
#include <QFileSystemModel>
#include <QItemSelection>
#include <QThread>
#include <QFileSystemWatcher>

#include "ui_mainwindow.h"
#include "extfsm.h"
#include "calculator.h"
#include "pairwidget.h"

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
    void setLoader(const QString loaderPath, const int file);
    void setLimits(const qreal left, const qreal right, const int file);
    void setEnergyShift(const qreal shift, const int file);
    void setShadowCurrent(const qreal signal, const qreal iZero, const int file);
    void setSmooth(const int count, const int file);
    void setDiff(const bool needed, const int file);
    void setLinearIntervals(const QPointF interval, bool needed, const int file);
    void setNormalizationCoeff(const qreal coeff, bool needed, const int file);
    void setStepped(const qreal coeff, bool needed, const int file);
    void setIntegrate(const bool needed, const int index, const int file);
    void setIntegrationConstants(const qreal newPc, const qreal newNh);
    void setCalculate(const bool needed, const QPointF newPhi, const QPointF newTheta);

private:
    QThread *calcThread;
    Calculator *calculator;
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
    void resizeEvent(QResizeEvent*);
    QtCharts::QChartView chartView;
    QtCharts::QChart chart;
    QtCharts::QLineSeries raw[2];
    QtCharts::QLineSeries norm[2];
    QtCharts::QLineSeries zero[2];
    QtCharts::QLineSeries xmcd;
    QtCharts::QLineSeries line[2];
    QtCharts::QValueAxis axisX;
    QtCharts::QValueAxis axisY;
    QtCharts::QValueAxis axisY2;
    QFileSystemModel model;
    ExtFSM *table = new ExtFSM(this);
    QString dataDir;
    QString filePath[2];
    QString fileName[2];
    qreal theta[2];
    qreal phi[2];
    qreal muB[4] = {9.274009994*pow(10,-24), 1, 5.7883818012*pow(10, -5), 9.274009994*pow(10, -21)};
    QString units[4] = {"J/T", "µβ", "eV/T", "erg/G"};
    QString sample = "not found";
    QString geom = "not found";
    QString energy = "not found";
    qreal angle[3] = {0, 55, 65};
    QSettings session;
    bool loaded[2];
    QLabel *fileNameLabel[2];
    QRadioButton *fileCheckBox[2];
    QCheckBox *holderBox[2];
    QDoubleSpinBox *offsets[2];
    QRadioButton *parallel[2];
    void rescale();
    QFileSystemWatcher *refresh;
    int id = 0;
    int file = 0;
    QList<PairWidget *> pairs;
    QString defaults();
    void loadState(QString state, const int file);//file = -1, 0 , 1// -1 -> ui changes only

private slots:
    void open(QString);
    void load(QModelIndex);
    void exportCharts();
    void forget(int buttonID);
    void myResize();//?
    void swap();
    void setPath();
    void newPair();
    void getData(const QVector<QPair<qreal, QPointF>> points);
    void rawData(const QVector<QPair<qreal, QPointF>> points);
    void iZero(const QVector<QPair<qreal, QPointF>> points);
    void XMCD(const QVector<QPointF> points);
    void linCoeffs(const QPointF left, const QPointF right);
    void integrals(const qreal summ, const qreal dl2, const qreal dl3, const qreal mSE, const qreal mO);
    void moments(const qreal mOP, const qreal mOO, const qreal ms, const qreal mt);
    void paintItBlack(const int id);
    void deletePair(const int id);
    void fileSelected(const int file);
    void saveSession();
};

#endif // MAINWINDOW_H
