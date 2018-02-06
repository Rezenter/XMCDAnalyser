#include "mainwindow.h"
#include <complex>
#include <QDateTime>
#include <QtMath>
#include <QStorageInfo>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    /*To-do list:
        * clear linear coeff. lables
        * carefully delete everything in destructor
        * prevent crash
        * save/load session
        * move path selection do a separate dialog
        * replace file tree with labels for each pair of files
        * refresh should also refresh right table
        * add sample filter
        * check "forget" buttons: not all checkboxes returned to their default state
        * fix "calculate" button: availiable before integration of both files
    */

    ui->setupUi(this);
    fileNameLabel[0] = ui->file1Label;
    fileNameLabel[1] = ui->file2Label;
    fileCheckBox[0] = ui->file1Button;
    fileCheckBox[1] = ui->file2Button;
    holderBox[0] = ui->holder1Box;
    holderBox[1] = ui->holder2Box;
    offsets[0] = ui->phi1OffsetSpinBox;
    offsets[1] = ui->phi2OffsetSpinBox;
    parallel[0] = ui->angle1Box;
    parallel[1] = ui->angle2Box;
    refresh = new QFileSystemWatcher(this);
    QSettings session(QApplication::applicationDirPath() + "/session.ini", QSettings::IniFormat);
    session.setValue("exePath", QCoreApplication::applicationDirPath());
    ui->tabWidget->setCurrentIndex(0);
    axisX.setMinorGridLineVisible(true);
    chart.setTitle("XAS");
    chart.setAxisX(&axisX);
    chart.setAxisY(&axisY);
    axisY.setGridLineColor(QColor(150, 250 , 150));
    axisY.setLinePenColor(QColor(0, 150 , 0));
    axisY2.setGridLineColor(QColor(150, 150 , 250));
    axisY2.setLinePenColor(QColor(0, 0 , 150));
    axisX.setMinorTickCount(4);
    axisX.setTickCount(20);
    axisX.setTitleText("primary photons energy, eV");
    axisY.setMinorTickCount(2);
    axisY.setTickCount(10);
    axisY.setTitleText("Intensity XAS, arb.u.");
    axisY2.setMinorTickCount(2);
    axisY2.setTickCount(11);
    axisY2.setTitleText("Intensity XMCD, arb.u.");
    chartView.setRenderHint(QPainter::Antialiasing);
    chartView.setParent(ui->chartWidget);
    chartView.setChart(&chart);
    chartView.show();
    for(int file = 0; file < 2; file++){
        norm[file].setName("Norm " + QString::number(file));
        raw[file].setName("Raw " + QString::number(file));
        zero[file].setName("Zero " + QString::number(file));
        line[file].setName("linear " + QString::number(file));
        chart.addSeries(&norm[file]);
        norm[file].attachAxis(&axisX);
        norm[file].attachAxis(&axisY);
        chart.addSeries(&raw[file]);
        raw[file].attachAxis(&axisX);
        raw[file].attachAxis(&axisY);
        raw[file].setVisible(false);
        chart.addSeries(&zero[file]);
        zero[file].attachAxis(&axisX);
        zero[file].attachAxis(&axisY);
        zero[file].setVisible(false);
        chart.addSeries(&line[file]);
        line[file].attachAxis(&axisX);
        line[file].attachAxis(&axisY);
        line[file].setVisible(false);
    }
    xmcd.setName("XMCD");
    chart.addSeries(&xmcd);
    xmcd.attachAxis(&axisX);
    xmcd.attachAxis(&axisY);
    xmcd.setVisible(false);
    ui->fileTable->setModel(table);
    ui->fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileTable->setSelectionMode(QAbstractItemView::SingleSelection);
    calcThread = new QThread();
    calculator = new Calculator();
    calculator->moveToThread(calcThread);
    QObject::connect(calculator, SIGNAL(dead()), calcThread, SLOT(quit()));
    QObject::connect(calcThread, SIGNAL(finished()), calculator, SLOT(deleteLater()));
    QObject::connect(calculator, SIGNAL(dead()), calcThread, SLOT(deleteLater()));
    /*
    QObject::connect(ui->elseGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->normGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->llSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->rlSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->backgroundGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->filletSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->pSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->nSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->shadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->zeroShadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->phi1OffsetBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->phi2OffsetBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->angleGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->sumBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    QObject::connect(ui->levelBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    */
    //QObject::connect(ui->treatAngleAsGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reopen);
    //QObject::connect(ui->unitBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::reCalcBoth);
    //QObject::connect(ui->energyShiftSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalcBoth);

    //ui to this
    QObject::connect(ui->fileTable, &QTableView::doubleClicked, this, static_cast<void(MainWindow::*)(QModelIndex)>(&MainWindow::load));
    QObject::connect(ui->file1Button, &QRadioButton::toggled, this, [=]{
        open(dataDir);
    });
    QObject::connect(ui->rawBox, &QRadioButton::toggled, this, [=](bool state){
        raw[0].setVisible(state);
        raw[1].setVisible(state);
        if(state){
            rescale();
        }
    });
    QObject::connect(ui->zeroBox, &QRadioButton::toggled, this, [=](bool state){
        zero[0].setVisible(state);
        zero[1].setVisible(state);
        if(state){
            rescale();
        }
    });
    QObject::connect(ui->normBox, &QRadioButton::toggled, this, [=](bool state){
        norm[0].setVisible(state);
        norm[1].setVisible(state);
        if(state){
            rescale();
            ui->smoothSpinBox->setMaximum(norm[0].pointsVector().size());
        }else{
            ui->mulBox->setChecked(false);
        }
        ui->shadowSpinBox->setEnabled(state);
        ui->zeroShadowSpinBox->setEnabled(state);
        ui->smoothSpinBox->setEnabled(state);
        ui->mulBox->setEnabled(state);
    });
    QObject::connect(ui->shadowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setShadowCurrent(val, ui->zeroShadowSpinBox->value(), file);
    });
    QObject::connect(ui->zeroShadowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setShadowCurrent(ui->shadowSpinBox->value(), val, file);
    });
    QObject::connect(ui->smoothSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        setSmooth(val, file);
    });
    QObject::connect(ui->mulBox, &QRadioButton::toggled, this, [=](bool state){
        ui->mulCoeffSpinBox->setEnabled(state);
        ui->linearBox->setEnabled(state);
        setNormalizationCoeff(ui->mulCoeffSpinBox->value(), state, file);
        if(!state){
            ui->linearBox->setEnabled(false);
        }
    });
    QObject::connect(ui->mulCoeffSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setNormalizationCoeff(val, ui->mulBox->isChecked(), file);
    });
    QObject::connect(ui->linearBox, &QRadioButton::toggled, this, [=](bool state){
        ui->llSpinBox->setEnabled(state);
        ui->rlSpinBox->setEnabled(state);
        ui->linearBackgroundBox->setEnabled(state);
        line[0].setVisible(state);
        line[1].setVisible(state);
        rescale();
    });

    QObject::connect(ui->exportButton, &QPushButton::pressed, this, &MainWindow::exportCharts);
    QObject::connect(ui->bSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{
        setLimits(ui->bSpinBox->value(), ui->tSpinBox->value(), file);
    });
    QObject::connect(ui->tSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{
        setLimits(ui->bSpinBox->value(), ui->tSpinBox->value(), file);
    });
    QObject::connect(ui->smoothSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i){
        setSmooth(i, file);
    });
    QObject::connect(ui->shadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]{
        setShadowCurrent(ui->shadowSpinBox->value(), ui->zeroShadowSpinBox->value(), file);
    });
    QObject::connect(ui->zeroShadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]{
        setShadowCurrent(ui->shadowSpinBox->value(), ui->zeroShadowSpinBox->value(), file);
    });
    QObject::connect(ui->energyShiftSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double d){setEnergyShift(d, file);});
    QObject::connect(ui->tabWidget, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), this, &MainWindow::myResize);
    QObject::connect(ui->swapButton, &QPushButton::pressed, this, &MainWindow::swap);
    //QObject::connect(ui->sessionButton, &QPushButton::pressed, this, &MainWindow::loadState);
    QObject::connect(ui->selectPathButton, &QPushButton::pressed, this, &MainWindow::setPath);
    QObject::connect(refresh, &QFileSystemWatcher::directoryChanged, this, &MainWindow::open);
    QObject::connect(ui->pairButton, &QPushButton::pressed, this, &MainWindow::newPair);

    //holder buttons!!!


    //this to calc
    QObject::connect(this, &MainWindow::setEnergyShift, calculator, &Calculator::setEnergyShift);
    QObject::connect(this, &MainWindow::setLimits, calculator, &Calculator::setLimits);
    QObject::connect(this, &MainWindow::setLoader, calculator, &Calculator::setLoader);
    QObject::connect(this, &MainWindow::setSmooth, calculator, &Calculator::setSmooth);
    QObject::connect(this, &MainWindow::setShadowCurrent, calculator, &Calculator::setShadowCurrent);
    QObject::connect(this, &MainWindow::setNormalizationCoeff, calculator, &Calculator::setNormalizationCoeff);

    //calc to this
    qRegisterMetaType<QVector<QPair<qreal,QPointF>>>("QVector<QPair<qreal,QPointF>>");
    QObject::connect(calculator, &Calculator::processedData, this, &MainWindow::getData);
    QObject::connect(calculator, &Calculator::rawData, this, &MainWindow::rawData);
    QObject::connect(calculator, &Calculator::iZero, this, &MainWindow::iZero);
    QObject::connect(calculator, &Calculator::XMCD, this, &MainWindow::XMCD);
    QObject::connect(calculator, &Calculator::linCoeffs, this, &MainWindow::linCoeffs);
    QObject::connect(calculator, &Calculator::integrals, this, &MainWindow::integrals);
    QObject::connect(calculator, &Calculator::moments, this, &MainWindow::moments);
    QObject::connect(calculator, &Calculator::compleated, this, &MainWindow::saveSession);

    loadSettings();
    ui->scrollAreaInternals->resize(ui->scrollArea->width() - 5, ui->scrollArea->height());
    if(session.value("normalExit", false).toBool()){
        //load all nods
        //loadState();//1
        //2
    }else{
        QString tmp = defaults();
        loadState(tmp, -1);
        QFile::remove(tmp);
    }
    session.setValue("normalExit", false);

    ui->sessionButton->setEnabled(false); //disabled in current version
    calcThread->start();
}

MainWindow::~MainWindow()// recheck the system
{
    delete refresh;
    calcThread->quit();
    calcThread->requestInterruption();
    if(calcThread->wait()){
        //qDebug() << "OK";
    }else{
        qDebug() << "thread error";
    }
    delete calcThread;
    //session.setValue("normalExit", true);//debug
    saveSettings();
    delete ui;
}

void MainWindow::newPair(){
    if(fileName[0].size() != 0 && fileName[1].size() != 0){
        pairs.append(new PairWidget());
        id = pairs.count() - 1;
        if(ui->scrollArea->height() > ui->scrollAreaInternals->height()){
            ui->scrollAreaInternals->resize(ui->scrollArea->width() - 5, ui->scrollArea->height());
        }
        if(100 * id >= ui->scrollArea->height()){
            ui->scrollAreaInternals->resize(230, ui->scrollAreaInternals->height() + 100);
        }
        ui->scrollAreaInternals->layout()->addWidget(pairs.last());
        pairs.last()->id = id;
        for(int file = 0; file < 2 ; ++file){
            fileNameLabel[file]->setText("");
            pairs.last()->state[file] = defaults();
            pairs.last()->fileLabels[file]->setText(fileName[file]);
            QSettings tmp(pairs.last()->state[file], QSettings::IniFormat);
            tmp.setValue("filename", filePath[file] + "/" + fileName[file]);
            tmp.setValue("isChecked", file == 0);
            fileName[file] = "";
            filePath[file] = "";
        }
        pairs.last()->ui.groupBox->setTitle(QString::number(id));//fix
        pairs.at(id)->fileButtons[0]->setChecked(true);
        ui->file1Button->setChecked(true);
        paintItBlack(id);
        QObject::connect(pairs.last(), &PairWidget::selected, this, &MainWindow::paintItBlack);
        QObject::connect(pairs.last(), &PairWidget::deletePressed, this, &MainWindow::deletePair);
        QObject::connect(pairs.last(), &PairWidget::fileSelected, this, &MainWindow::fileSelected);
    }
}

void MainWindow::fileSelected(const int file){
    loadState(pairs.at(id)->state[file], file);
}

void MainWindow::paintItBlack(const int id){
    for(int i = 0; i < pairs.size(); ++i){
        if(id != i){
            pairs.at(i)->setStyleSheet("background-color: white;");
        }else{
            pairs.at(i)->setStyleSheet("background-color: rgb(200, 200, 220);");
        }
    }
    this->id = id;
    if(pairs.at(id)->ui.file1Button->isChecked()){
        file = 0;
    }else{
        file = 1;
    }
    loadState(pairs.at(id)->state[file], file);
}

void MainWindow::deletePair(const int id){
    if(pairs.size() > 1){
        if(100 * this->id > ui->scrollArea->height() - 36){
            ui->scrollAreaInternals->resize(230, ui->scrollAreaInternals->height() - 100);
        }
        ui->scrollAreaInternals->layout()->removeWidget(pairs.at(id));
        for(int i = id; i < pairs.size(); i++){
           pairs.at(i)->id--;
        }
        QFile::remove(pairs.takeAt(id)->state[0]);
        QFile::remove(pairs.takeAt(id)->state[1]);
        delete pairs.takeAt(id);
        if(id == this->id){
            if(id != 0){
                paintItBlack(id - 1);
            }else{
                paintItBlack(0);
            }
        }
    }else{
        //fix
        file = 0;
        QString tmp = defaults();
        loadState(tmp, -1);
        QFile::remove(tmp);
    }
}

void MainWindow::rawData(const QVector<QPair<qreal, QPointF>> points){
    raw[0].blockSignals(true);
    raw[0].clear();
    raw[1].blockSignals(true);
    raw[1].clear();
    for(int i = 0; i < points.size(); ++i){
        raw[0].append(points[i].first, points[i].second.x());
        raw[1].append(points[i].first, points[i].second.y());
    }
    raw[0].blockSignals(false);
    raw[1].blockSignals(false);
    raw[0].pointsReplaced();
    raw[1].pointsReplaced();
}

void MainWindow::iZero(const QVector<QPair<qreal, QPointF>> points){
    zero[0].blockSignals(true);
    zero[0].clear();
    zero[1].blockSignals(true);
    zero[1].clear();
    for(int i = 0; i < points.size(); ++i){
        zero[0].append(points[i].first, points[i].second.x());
        zero[1].append(points[i].first, points[i].second.y());
    }
    zero[0].blockSignals(false);
    zero[1].blockSignals(false);
    zero[0].pointsReplaced();
    zero[1].pointsReplaced();
}

void MainWindow::XMCD(const QVector<QPointF> points){
    xmcd.blockSignals(true);
    xmcd.clear();
    for(int i = 0; i < points.size(); ++i){
        xmcd.append(points[i].x(), points[i].y());
    }
    xmcd.blockSignals(false);
}

void MainWindow::linCoeffs(const QPointF left, const QPointF right){
    line[0].blockSignals(true);
    line[0].clear();
    line[1].blockSignals(true);
    line[1].clear();
    line[0].append(norm[0].at(0).x(), norm[0].at(0).x() * left.x() + left.y());
    line[0].append(norm[0].at(norm[0].points().size()).x(), norm[0].at(norm[0].points().size()).x() * left.x() + left.y());
    line[1].append(norm[1].at(0).x(), norm[1].at(0).x() * right.x() + right.y());
    line[1].append(norm[1].at(norm[1].points().size()).x(), norm[1].at(norm[1].points().size()).x() * right.x() + right.y());
    line[0].blockSignals(false);
    line[1].blockSignals(false);
    line[0].pointsReplaced();
    line[1].pointsReplaced();
}

void MainWindow::integrals(const qreal summ, const qreal dl2, const qreal dl3, const qreal mSE, const qreal mO){

}

void MainWindow::moments(const qreal mOP, const qreal mOO, const qreal ms, const qreal mt){

}

void MainWindow::getData(const QVector<QPair<qreal, QPointF> > points){
    norm[0].blockSignals(true);
    norm[0].clear();
    norm[1].blockSignals(true);
    norm[1].clear();
    for(int i = 0; i < points.size(); ++i){
        norm[0].append(points[i].first, points[i].second.x());
        norm[1].append(points[i].first, points[i].second.y());
    }
    norm[0].blockSignals(false);
    norm[1].blockSignals(false);
    norm[0].pointsReplaced();
    norm[1].pointsReplaced();
}

void MainWindow::rescale(){
    QPointF x(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QPointF y(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QtCharts::QLineSeries *lineSer;
    QList<QtCharts::QAbstractSeries*>::iterator end = chart.series().end()--;
    std::for_each(chart.series().begin(), end, [&x, &y, &lineSer, this](QtCharts::QAbstractSeries* const ser){//"this" for debug
        if(ser->isVisible()){
            if(ser){
                lineSer = dynamic_cast<QtCharts::QLineSeries *>(ser);
            }
            if(lineSer){
                QVector<QPointF> tmp = lineSer->pointsVector();
                for(int i = 0; i < tmp.size(); i++){
                    if(tmp[i].y() > y.y()){
                        y.ry() = tmp[i].y();
                    }
                    if(tmp[i].y() < y.x()){
                        y.rx() = tmp[i].y();
                    }
                    if(tmp[i].x() > x.y()){
                        x.ry() = tmp[i].x();
                    }
                    if(tmp[i].x() < x.x()){
                        x.rx() = tmp[i].x();
                    }
                }
            }else{
                qDebug() << "error in " << objectName() << "::" << __FUNCTION__  << "unable to cast to QLineSeries";
            }
        }
    });
    if(axisX.min() != x.x() && axisX.max() != x.y()){
        axisX.setRange(x.x(), x.y());
    }else if(axisX.min() != x.x()){
        axisX.setMin(x.x());
    }else if(axisX.max() != x.y()){
        axisX.setMax(x.y());
    }
    if(axisY.min() != y.x() && axisY.max() != y.y()){
        axisY.setRange(y.x(), y.y());
    }else if(axisY.min() != y.x()){
        axisY.setMin(y.x());
    }else if(axisY.max() != y.y()){
        axisY.setMax(y.y());
    }
}

void MainWindow::myResize(){
    chartView.resize(ui->chartWidget->size());
}

void MainWindow::swap(){
    //fix
}

void MainWindow::loadSettings(){
    QSettings settings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.beginGroup("gui");
        resize(settings.value("windowSize", QSize(800, 600)).toSize());
        move(settings.value("windowPosition", QPoint(0, 0)).toPoint());
        setWindowTitle(settings.value("windowTitle", "XMCDAnalyzer(corrupted .ini)").toString());
        dataDir = settings.value("lastDir", "").toString();
        settings.beginGroup("fileTable");
            ui->fileTable->setColumnWidth(0, settings.value("nameWidth", 200).toInt());
            ui->fileTable->setColumnWidth(1, settings.value("sampleWidth", 200).toInt());
            ui->fileTable->setColumnWidth(2, settings.value("geometryWidth", 200).toInt());
         ui->fileTable->setColumnWidth(4, settings.value("commentWidth", 200).toInt());
         ui->fileTable->setColumnWidth(5, settings.value("dateWidth", 200).toInt());
         ui->fileTable->setColumnWidth(3, settings.value("elementWidth", 200).toInt());
        settings.endGroup();
        settings.beginGroup("chart");
            ui->chartWidget->resize(settings.value("lastSize", QSize(800, 600)).toSize());
        settings.endGroup();
    settings.endGroup();
    myResize();
}

void MainWindow::saveSettings(){
    QSettings settings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.beginGroup("gui");
        settings.setValue("windowSize", this->size());
        settings.setValue("windowPosition", this->pos());
        settings.setValue("windowTitle", this->windowTitle());
        settings.setValue("lastDir", dataDir);
        settings.beginGroup("fileTable");
            settings.setValue("nameWidth", ui->fileTable->columnWidth(0));
            settings.setValue("sampleWidth", ui->fileTable->columnWidth(1));
            settings.setValue("geometryWidth", ui->fileTable->columnWidth(2));
            settings.setValue("elementWidth", ui->fileTable->columnWidth(3));
            settings.setValue("dateWidth", ui->fileTable->columnWidth(5));
            settings.setValue("commentWidth", ui->fileTable->columnWidth(4));
        settings.endGroup();
        settings.beginGroup("chart");
            settings.setValue("lastSize", ui->chartWidget->size());
        settings.endGroup();
    settings.endGroup();
}

void MainWindow::open(QString selectedPath){
    if(selectedPath.length() != 0){
        if(ui->file1Button->isChecked()){
            filePath[0] = selectedPath;
        }else{
            filePath[1] = selectedPath;
        }
        table->removeAll();
        QDir currentDir(selectedPath);
        QStringList filters;
        filters << "*.TXT";
        QFileInfoList files = currentDir.entryInfoList(filters, QDir::Files, QDir::Name);
        int numRows = files.size();
        int row = 0;
        for (int i = 0; i < numRows; ++i) {
            QString tmp = files.at(i).filePath();
            QFile currentFile(tmp);
            currentFile.open(QIODevice::ReadOnly);
            QString line = QTextStream(&currentFile).readLine();
            currentFile.close();
            bool sameEnergy = true;
            if(ui->energyBox->isChecked() && (energy != line.split("	").first())){
                sameEnergy = false;
            }
            tmp.chop(3);
            QSettings par(tmp + "par", QSettings::IniFormat);
            tmp = files.at(i).fileName();
            tmp.chop(4);
            par.beginGroup(tmp);
            bool sameSample = true;
            if(ui->sampleBox->isChecked() && (par.value("sampleName", "no info").toString() != sample)){
                sameSample = false;
            }
            bool sameGeom = true;
            if(ui->geomBox->isChecked() && ((par.value("angle", "-1").toString() == "-1") || (par.value("angle", "-1").toString() == "") || (par.value("angle", "noInfo").toString() == geom))){
                sameGeom = false;
            }
            if(((fileName[0].size() != 0) && sameSample && sameGeom && sameEnergy) || ui->file1Button->isChecked()){
                table->insertRow(row, QModelIndex());
                QModelIndex index = table->index(row, 0, QModelIndex());
                table->setData(index, files.at(i).fileName(), Qt::EditRole);
                index = table->index(row, 1, QModelIndex());
                table->setData(index, par.value("sampleName", "no info") , Qt::EditRole);
                index = table->index(row, 2, QModelIndex());
                tmp = par.value("angle", "-1").toString();
                if(tmp == ""){
                    tmp = "-1";
                }
                table->setData(index, tmp , Qt::EditRole);
                index = table->index(row, 3, QModelIndex());
                table->setData(index, par.value("rating", "no info"), Qt::EditRole);
                index = table->index(row, 4, QModelIndex());
                table->setData(index, par.value("comment", "no info"), Qt::EditRole);
                index = table->index(row, 5, QModelIndex());
                table->setData(index, files.at(i).lastModified().toString("dd.MM.yyyy   HH:m"), Qt::EditRole);
                row++;
            }
            par.endGroup();
        }
    }
}

void MainWindow::load(QModelIndex index){
    int file = 1;
    if(fileCheckBox[0]->isChecked()){
        file = 0;
        sample = table->data(table->index(index.row(), 1), Qt::DisplayRole).toString();
        ui->exportLine->setText(filePath[file] + "/" + sample + "_output");
        geom = table->data(table->index(index.row(), 2), Qt::DisplayRole).toString();
        QFile currentFile(filePath[file] + "/" + table->data(table->index(index.row(), 0), Qt::DisplayRole).toString());
        currentFile.open(QIODevice::ReadOnly);
        QTextStream stream(&currentFile);
        QString line = stream.readLine();
        currentFile.close();
        energy = line.split("	").first();
    }
    fileName[file] = table->data(table->index(index.row(), 0), Qt::DisplayRole).toString();
    fileNameLabel[file]->setText(fileName[file]);
    /*
    emit setLoader(filePath[file] + "/" + fileName[file], file);
    theta[file] = table->data(index = table->index(index.row(), 2), Qt::DisplayRole).toDouble();
    if(holderBox[file]->isChecked()){
        theta[file] = angle[(int)theta[file]];//create reverse transaction
    }
    theta[file] = qDegreesToRadians(theta[file]);
    loaded[file] = true;
    */
}

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    chartView.resize(ui->chartWidget->size());
}

void MainWindow::exportCharts(){
    QString exPath = ui->exportLine->text();
    if(!QDir(exPath).exists()){
        QDir().mkdir(exPath);
    }
    QFile outFile(exPath + "/common.txt");
    outFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&outFile);
    //...
    outFile.close();

    QSettings expSession(exPath + "/session.ini", QSettings::IniFormat);
    //...
}

void MainWindow::forget(int file){
    loaded[file] = false;
    QString tmp = defaults();
    loadState(tmp, file);
    if(pairs.size() != 0){
        QFile::remove(pairs.at(id)->state[file]);
        pairs.at(id)->state[file] = tmp;
    }
}

void MainWindow::loadState(QString session, const int file){
    QSettings *set = new QSettings(session, QSettings::IniFormat);
    if(file == 0 || file == 1){
        fileName[file] = set->value("filename", "").toString();
        QString tmp = fileName[file].split("/").last();
        pairs.at(id)->fileLabels[file]->setText(tmp);
        filePath[file] = fileName[file].chopped(tmp.size() + 1);
        if(set->value("isChecked", false).toBool()){
            pairs.at(id)->fileButtons[file]->setChecked(true);
            setLoader(fileName[file], file);
        }
    }
    ui->rawBox->setChecked(set->value("isRaw", false).toBool());
    ui->zeroBox->setChecked(set->value("isZero", false).toBool());
    ui->normBox->setChecked(set->value("isNorm", true).toBool()); //default
    ui->shadowSpinBox->setValue(set->value("shadow", 0.0).toDouble());
    ui->zeroShadowSpinBox->setValue(set->value("zeroShadow", 0.0).toDouble());
    ui->mulBox->setChecked(set->value("isMult", false).toBool());
    ui->mulCoeffSpinBox->setValue(set->value("mulCoeff", 0.0).toDouble());
    ui->smoothSpinBox->setValue(set->value("smooth", 0).toInt());
    ui->bSpinBox->setValue(set->value("cropL", 0).toInt());
    ui->tSpinBox->setValue(set->value("cropR", 0).toInt());
    ui->linearBox->setChecked(set->value("linearShow", false).toBool());
    ui->llSpinBox->setValue(set->value("linearL", 0.0).toDouble());
    ui->rlSpinBox->setValue(set->value("linearR", 0.0).toDouble());
    ui->filletSpinBox->setValue(set->value("steppedMul", 10.0).toDouble());
    ui->linearBackgroundBox->setChecked(set->value("linear", false).toBool());
    ui->steppedBackgroundBox->setChecked(set->value("stepped", false).toBool());
    ui->diffBox->setChecked(set->value("xmcd", false).toBool());
    ui->integrationLimitSpinBox->setValue(set->value("integrationLimit", 0).toInt());
    ui->integrateBox->setChecked(set->value("integrate", false).toBool());
    ui->calculateBox->setChecked(set->value("calc", false).toBool());
    if(file == 0 || file == 1){
        offsets[file]->setValue(set->value("phi1", 0.0).toDouble());
        parallel[file]->setChecked(set->value("phiChecked", false).toBool());
        ui->rawBox->setEnabled(true);
        ui->zeroBox->setEnabled(true);
        ui->normBox->setEnabled(true);
    }else{
        for(int i = 0; i < 2; ++i){
            offsets[i]->setValue(set->value("phi1", 0.0).toDouble());
            parallel[i]->setChecked(set->value("phiChecked", false).toBool());
        }
    }
    bool state = ui->normBox->isChecked() && (file == 0 || file == 1);
    norm[0].setVisible(state);
    norm[1].setVisible(state);
    if(state){
        rescale();
        ui->smoothSpinBox->setMaximum(norm[0].pointsVector().size());
    }
    ui->mulBox->setChecked(false);
    ui->shadowSpinBox->setEnabled(state);
    ui->zeroShadowSpinBox->setEnabled(state);
    ui->smoothSpinBox->setEnabled(state);
    ui->mulBox->setEnabled(state);
    delete set;
}

QString MainWindow::defaults(){
    QString res = QApplication::applicationDirPath() + "/tmp/";
    QString name = QString::number(id);
    int diff = 'z'-'a';
    while(QFile::exists(res + name + ".ini")){
        name = QString::number(id);
        for(int i = 0; i < 8; ++i){
            char c = 'a'+(rand() % diff);
            name += QChar(c);
        }
    }
    res += name + ".ini";
    QSettings *set = new QSettings(res, QSettings::IniFormat);
    set->setValue("filename", "");
    set->setValue("isChecked", false);
    set->setValue("isRaw", false);
    set->setValue("isZero", false);
    set->setValue("isNorm", true);
    set->setValue("shadow", 0.0);
    set->setValue("zeroShadow", 0.0);
    set->setValue("isMult", false);
    set->setValue("mulCoeff", 0.0);
    set->setValue("smooth", 1);
    set->setValue("cropL", 0);
    set->setValue("cropR", 0);
    set->setValue("linearShow", false);
    set->setValue("linearL", 0.0);
    set->setValue("linearR", 0.0);
    set->setValue("steppedMul", 10.0);
    set->setValue("linear", false);
    set->setValue("stepped", false);
    set->setValue("xmcd", false);
    set->setValue("integrationLimit", 0);
    set->setValue("integrate", false);
    set->setValue("calc", false);
    set->setValue("phi", 0.0);
    set->setValue("phiChecked", false);
    delete set;
    return res;
}

void MainWindow::saveSession(){
    rescale();
    //save here
}

void MainWindow::setPath(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select data directory"), dataDir);
    if(dir.length() != 0){
        refresh->removePath(dataDir);
        dataDir = dir;
        open(dir);
        refresh->addPath(dir);
    }
}
