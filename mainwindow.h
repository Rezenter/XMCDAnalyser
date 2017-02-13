#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    FileLoader *dataLoader;
    QString filePath;
    QVector<QPair<qreal, QPair<qreal, qreal>>> data;
    QVector<QPair<qreal, QPair<qreal, qreal>>> bareData;
    QPair<int, int> lInd;
    QPair<int, int> rInd;
    QString fileName;
    QFile *outFile;
    QTextStream *outStream;
    bool mul = true;
    qreal a;
    qreal b;
    QtCharts::QLineSeries *stepsSeries;
    qreal Nh = 2.49;
    qreal Pc = 0.83;
    qreal mOrb = 0;
    qreal msEff = 0;
    qreal muB = 1;

private slots:
    void refreshButton();
    void driveChanged(int);
    void open(QModelIndex);
    void load(QModelIndex);
    void load();
    QModelIndex selected(QModelIndex);
    void reCalc();
    void findMax();
    void exportCharts();
    void fullNorm();
    void lChanged();
    void bg();
};

#endif // MAINWINDOW_H
