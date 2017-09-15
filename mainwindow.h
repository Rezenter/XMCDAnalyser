#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QGenericMatrix>
#include <QMainWindow>
#include <QRegExpValidator>
#include <QtMath>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>
#include <QThread>
#include <QCoreApplication>
#include <QSettings>
#include <QStorageInfo>
#include <QFileSystemModel>
#include "extfsm.h"
#include <QItemSelection>
#include "fileloader.h"
#include <QtMath>
#include <QHash>

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

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
    void refresh();
    void buildFileTree();
    void resizeEvent(QResizeEvent*);
    void saveSession();
    QtCharts::QChartView *chartView;
    QtCharts::QChartView *diffView;
    QtCharts::QChart *chart;
    QtCharts::QChart *diff;
    QtCharts::QLineSeries *l;
    QtCharts::QLineSeries *r;
    QtCharts::QLineSeries *lNorm;
    QtCharts::QLineSeries *rNorm;
    QtCharts::QLineSeries *lSteps;
    QtCharts::QLineSeries *rSteps;
    QtCharts::QLineSeries *diffS;
    QtCharts::QLineSeries *lMax;
    QtCharts::QLineSeries *rMax;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;
    QtCharts::QValueAxis *diffY;
    QtCharts::QValueAxis *diffX;
    QtCharts::QLineSeries *divider;
    QtCharts::QLineSeries *lz;
    QtCharts::QLineSeries *rz;
    QtCharts::QLineSeries *l0;
    QtCharts::QLineSeries *lIntervals;
    QtCharts::QLineSeries *ll;
    QtCharts::QLineSeries *rl;
    QtCharts::QLineSeries *level;
    QString path = QCoreApplication::applicationDirPath();
    QFile *save;
    QTextStream *stream;
    QSettings *settings;
    QSettings *par;
    QStorageInfo storage = QStorageInfo::root();
    QFileSystemModel *model = new QFileSystemModel();
    ExtFSM *table = new ExtFSM(this);
    QString drive;
    QModelIndex currentSelection;
    QString file1Path;
    QString file1Name;
    FileLoader *data1Loader;
    qreal teta1;
    QPair<qreal, qreal> limits1;
    QVector<QPair<qreal, QPair<qreal, qreal>>> tmp1Data;
    QString file2Path;
    QString file2Name;
    FileLoader *data2Loader;
    qreal teta2;
    QPair<qreal, qreal> limits2;
    QVector<QPair<qreal, QPair<qreal, qreal>>> tmp2Data;
    QVector<QPair<qreal, QPair<qreal, qreal>>> data;
    QVector<QPair<qreal, QPair<qreal, qreal>>> bareData;
    QPair<int, int> lInd;
    QPair<int, int> rInd;
    QFile *outFile;
    QTextStream *outStream;
    qreal a;
    qreal b;
    QtCharts::QLineSeries *stepsSeries;
    qreal mOrb1 = 0;
    qreal msEff1 = 0;
    qreal mOrb2 = 0;
    qreal msEff2 = 0;
    qreal muB[4] = {9.274009994*pow(10,-24), 1, 5.7883818012*pow(10, -5), 9.274009994*pow(10, -21)};
    QString units[4] = {"J/T", "µβ", "eV/T", "erg/G"};
    QString sample = "not found";
    QString geom = "not found";
    QString energy = "not found";
    bool loaded1 = false;
    bool loaded2 = false;
    bool calc1 = false;
    bool calc2 = false;
    qreal angle0 = 0;
    qreal angle1 = 55;
    qreal angle2 = 65;
    qreal ms;
    qreal mt;
    qreal mop;
    qreal moo;
    QHash<QString,qreal> *state;
    qreal phi1;
    qreal phi2;
    qreal eps = 0.01;
    QPair<qreal, qreal> intens;
    QSettings *session;
    QSettings *expSession;
    int exportState = 0;
    bool is1;


private slots:
    void refreshButton();
    void driveChanged(int);
    void open(QModelIndex);
    void load(QModelIndex);
    void reopen();
    void load();
    QModelIndex selected(QModelIndex);
    void reCalc();
    void findMax();
    void exportCharts();
    void lChanged();
    void bg();
    void fileSelect();
    void forget1();
    void forget2();
    void reCalcBoth();
    void myResize();//?
    void swap();
    void loadSession();
};

#endif // MAINWINDOW_H
