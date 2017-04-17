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
    QtCharts::QLineSeries *l;
    QtCharts::QLineSeries *r;
    QtCharts::QLineSeries *lNorm;
    QtCharts::QLineSeries *rNorm;
    QtCharts::QLineSeries *lSteps;
    QtCharts::QLineSeries *rSteps;
    QtCharts::QLineSeries *diffS;
    QtCharts::QLineSeries *lMax;
    QtCharts::QLineSeries *rMax;
    QtCharts::QChart *chart;
    QtCharts::QChart *diff;
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
    QString path = QCoreApplication::applicationDirPath();
    QFile *save;
    QTextStream *stream;
    QSettings *settings;
    QSettings *par;
    void loadSettings();
    void saveSettings();
    void refresh();
    QStorageInfo storage = QStorageInfo::root();
    void buildFileTree();
    QFileSystemModel *model = new QFileSystemModel();
    ExtFSM *table = new ExtFSM(this);
    QString drive;
    QModelIndex currentSelection;
    QtCharts::QChartView *chartView;
    QtCharts::QChartView *diffView;
    void resizeEvent(QResizeEvent*);
    FileLoader *data1Loader;
    FileLoader *data2Loader;
    QString file1Path;
    QString file2Path;
    QVector<QPair<qreal, QPair<qreal, qreal>>> data;
    QVector<QPair<qreal, QPair<qreal, qreal>>> bareData;
    QPair<int, int> lInd;
    QPair<int, int> rInd;
    QString file1Name;
    QString file2Name;
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
    QString sample = "null";
    QString geom = "null";
    bool loaded1 = false;
    bool loaded2 = false;
    bool calc1 = false;
    bool calc2 = false;
    bool changed1 = true;
    bool changed2 = true;
    qreal teta1;
    qreal teta2;
    qreal angle0 = 0;
    qreal angle1 = 55;
    qreal angle2 = 65;
    qreal ms;
    qreal mt;
    qreal mop;
    qreal moo;
    QHash<QString,qreal> *state;
    qreal lChopPrev;
    qreal rChopPrev;
    qreal lLinearPrev;
    qreal rLinearPrev;
    qreal dividerPrev;
    qreal smoothPrev;


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
};

#endif // MAINWINDOW_H
