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
    fileCheckBox[0] = ui->file1Box;
    fileCheckBox[1] = ui->file2Box;
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
        chart.addSeries(&norm[file]); //order is important!
        norm[file].attachAxis(&axisX);
        norm[file].attachAxis(&axisY);
    }

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
    QObject::connect(ui->fileTable, &QTableView::clicked, this, &MainWindow::selected);
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
    QObject::connect(ui->forgetGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, forget);
    QObject::connect(ui->tabWidget, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), this, &MainWindow::myResize);
    QObject::connect(ui->swapButton, &QPushButton::pressed, this, &MainWindow::swap);
    //QObject::connect(ui->sessionButton, &QPushButton::pressed, this, &MainWindow::loadState);
    QObject::connect(ui->selectPathButton, &QPushButton::pressed, this, &MainWindow::setPath);
    QObject::connect(refresh, &QFileSystemWatcher::directoryChanged, this, &MainWindow::open);
    QObject::connect(ui->pairButton, &QPushButton::pressed, this, &MainWindow::newPair);

    //this to calc
    QObject::connect(this, &MainWindow::setEnergyShift, calculator, &Calculator::setEnergyShift);
    QObject::connect(this, &MainWindow::setLimits, calculator, &Calculator::setLimits);
    QObject::connect(this, &MainWindow::setLoader, calculator, &Calculator::setLoader);
    QObject::connect(this, &MainWindow::setSmooth, calculator, &Calculator::setSmooth);
    QObject::connect(this, &MainWindow::setShadowCurrent, calculator, &Calculator::setShadowCurrent);

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
    if(!session.value("normalExit", false).toBool()){
        //load all nods
        //loadState();//1
        //2
    }else{
        QString tmp = defaults();
        loadState(tmp);
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
    pairs.last()->ui.groupBox->setTitle(QString::number(id));
    for(int file = 0; file < 2 ; ++file){
        pairs.last()->state[file] = defaults();
    }
    paintItBlack(id);
    QObject::connect(pairs.last(), &PairWidget::selected, this, &MainWindow::paintItBlack);
    QObject::connect(pairs.last(), &PairWidget::deletePressed, this, &MainWindow::deletePair);
    QObject::connect(pairs.last(), &PairWidget::fileSelected, this, &MainWindow::fileSelected);
}

void MainWindow::fileSelected(const int file){
    this->file = file;
    loadState(pairs.at(id)->state[file]);
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
    loadState(pairs.at(id)->state[file]);
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
        QString tmp = defaults();
        loadState(tmp);
        QFile::remove(tmp);
    }
}

void MainWindow::rawData(const QVector<QPair<qreal, QPointF>> points, const int file){

}

void MainWindow::iZero(const QVector<QPair<qreal, QPointF>> points, const int file){

}

void MainWindow::XMCD(const QVector<QPointF> points, const int file){

}

void MainWindow::linCoeffs(const QPointF left, const QPointF right, const int file){

}

void MainWindow::integrals(const qreal summ, const qreal dl2, const qreal dl3, const qreal mSE, const qreal mO, const int file){

}

void MainWindow::moments(const qreal mOP, const qreal mOO, const qreal ms, const qreal mt){

}

void MainWindow::getData(const QVector<QPair<qreal, QPointF> > points, const int file){
    //qint64 begin = QDateTime().currentMSecsSinceEpoch();
    norm[0].blockSignals(true);
    norm[1].blockSignals(true);
    norm[0].clear();
    norm[1].clear();
    for(int i = 0; i < points.size(); ++i){
        norm[0].append(points[i].first, points[i].second.x());
        norm[1].append(points[i].first, points[i].second.y());
    }
    norm[0].blockSignals(false);
    norm[1].blockSignals(false);
    rescale();//remove later
    //qDebug() << QDateTime().currentMSecsSinceEpoch() - begin << "rescale";
}

void MainWindow::rescale(){
    QPointF x(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QPointF y(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QtCharts::QLineSeries *lineSer;
    QList<QtCharts::QAbstractSeries*>::iterator end = chart.series().end()--;
    std::for_each(chart.series().begin(), end, [&x, &y, &lineSer, this](QtCharts::QAbstractSeries* const ser){//"this" for debug
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
        if(settings.value("useHolders", false).toBool()){
            ui->holderButton->setChecked(true);
        }else{
            ui->angleButton->setChecked(true);
        }
        //path
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
        settings.setValue("useHolders", ui->holderButton->isChecked());
        //
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
        if(ui->file1Box->isChecked()){
            filePath = selectedPath;
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
            if(((loaded[0]) && sameSample && sameGeom && sameEnergy) || ui->file1Box->isChecked()){
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
        ui->exportLine->setText(filePath + "/" + sample + "_output");
        geom = table->data(table->index(index.row(), 2), Qt::DisplayRole).toString();
        QFile currentFile(filePath + "/" + fileName[0]);
        currentFile.open(QIODevice::ReadOnly);
        QTextStream stream(&currentFile);
        QString line = stream.readLine();
        currentFile.close();
        energy = line.split("	").first();
    }
    fileName[file] = table->data(table->index(index.row(), 0), Qt::DisplayRole).toString();
    fileNameLabel[file]->setText(fileName[file]);
    emit setLoader(filePath + "/" + fileName[file], file);
    theta[file] = table->data(index = table->index(index.row(), 2), Qt::DisplayRole).toDouble();
    if(ui->holderButton->isChecked()){
        theta[file] = angle[(int)theta[file]]; //mb not here
    }
    theta[file] = qDegreesToRadians(theta[file]); //mb not here
    loaded[file] = true;
}

QModelIndex MainWindow::selected(QModelIndex index){
    return index;
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
    fileNameLabel[file]->setText("none");
    loaded[file] = false;
    fileName[file] = "";
    QString tmp = defaults();
    loadState(tmp);
    if(pairs.size() != 0){
        QFile::remove(pairs.at(id)->state[file]);
        pairs.at(id)->state[file] = tmp;
    }
}

void MainWindow::loadState(QString session){
    QSettings *set = new QSettings(session, QSettings::IniFormat);
    //value

    if(pairs.size() != 0){
        if(pairs.at(id)->ui.file1Button->isChecked()){
            pairs.at(id)->ui.file1Label->setText(set->value("filename", "unknown").toString());
        }else{
            pairs.at(id)->ui.file2Label->setText(set->value("filename", "unknown").toString());
        }
    }

    set->setValue("path", "");
    set->setValue("isChecked", false);
    set->setValue("isRaw", false);
    set->setValue("isZero", false);
    set->setValue("isNorm", true);
    set->setValue("shadow", 0.0);
    set->setValue("zeroShadow", 0.0);
    set->setValue("isMult", false);
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
    set->setValue("integrate", false);
    set->setValue("calc", false);
    set->setValue("phi1", 0.0);
    set->setValue("phi2", 0.0);
    set->setValue("shift", 0.0);
    set->setValue("angle=holder", false);
    delete test;
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
    set->setValue("path", "");
    set->setValue("isChecked", false);
    set->setValue("isRaw", false);
    set->setValue("isZero", false);
    set->setValue("isNorm", true);
    set->setValue("shadow", 0.0);
    set->setValue("zeroShadow", 0.0);
    set->setValue("isMult", false);
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
    set->setValue("integrate", false);
    set->setValue("calc", false);
    set->setValue("phi1", 0.0);
    set->setValue("phi2", 0.0);
    set->setValue("shift", 0.0);
    set->setValue("angle=holder", false);
    delete set;
    return res;
}

void MainWindow::saveSession(){
    qDebug() << "yep";
}

void MainWindow::setPath(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select data directory"),
                                                        dataDir,
                                                         QFileDialog::DontUseNativeDialog);
    if(dir.length() != 0){
        refresh->removePath(dataDir);
        dataDir = dir;
        open(dir);
        refresh->addPath(dir);
    }
}
