#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <complex>
#include <qdebug>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    par = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    ui->tabWidget->setCurrentIndex(0);
    chart = new QtCharts::QChart();
    diff = new QtCharts::QChart();
    axisX = new QtCharts::QValueAxis;
    axisX->setMinorGridLineVisible(true);
    diffX = new QtCharts::QValueAxis;
    diffX->setMinorGridLineVisible(true);
    chart->setTitle("XAS");
    diff->setTitle("XMCD");
    axisY = new QtCharts::QValueAxis();
    diffY = new QtCharts::QValueAxis();
    chartView = new QtCharts::QChartView(chart, ui->chartWidget);
    diffView = new QtCharts::QChartView(diff, ui->diffWidget);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->show();
    diffView->setRenderHint(QPainter::Antialiasing);
    diffView->show();
    ui->fileTable->setModel(table);
    ui->fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileTable->setSelectionMode(QAbstractItemView::SingleSelection);

    QObject::connect(ui->refreshButton, &QPushButton::pressed, this, &MainWindow::refreshButton);
    QObject::connect(ui->driveBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::driveChanged);
    QObject::connect(ui->fileMask, &QLineEdit::editingFinished, this, &MainWindow::refresh);
    QObject::connect(ui->fileTree, &QTreeView::doubleClicked, this, &MainWindow::open);
    QObject::connect(ui->fileTable, &QTableView::doubleClicked, this, static_cast<void(MainWindow::*)(QModelIndex)>(&MainWindow::load));
    QObject::connect(ui->loadButton, &QPushButton::pressed, this, static_cast<void(MainWindow::*)()>(&MainWindow::load));
    QObject::connect(ui->fileTable, &QTableView::clicked, this, &MainWindow::selected);
    QObject::connect(ui->llSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->rlSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->bSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->tSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->dividerSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->bSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->tSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->dividerSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->bothGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->exportButton, &QPushButton::pressed, this, &MainWindow::exportCharts);
    QObject::connect(ui->elseGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->normGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::fullNorm);
    QObject::connect(ui->dividerBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    QObject::connect(ui->llSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->rlSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->backgroundGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::bg);
    QObject::connect(ui->backgroundGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
     QObject::connect(ui->filletSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    buildFileTree();
    loadSettings();
    refresh();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete model;
    QtCharts::QAbstractSeries *delTmp;
    foreach(delTmp, chart->series()){
        delete &delTmp;
    }
    delete chart;
    foreach(delTmp, diff->series()){
        delete &delTmp;
    }
    delete diff;
    delete chartView;
    delete diffView;
    //delete axisX;
    //delete axisY;
    delete ui;
    delete par;
    delete settings;
}

void MainWindow::loadSettings(){
    settings->beginGroup("gui");
    resize(settings->value("windowSize", QSize(800, 600)).toSize());
    move(settings->value("windowPosition", QPoint(0, 0)).toPoint());
    setWindowTitle(settings->value("windowTitle", "XMCDAnalyser(corrupted .ini)").toString());
    drive = (settings->value("lastDrive", "C:/").toString());
    model->setRootPath(drive);
    settings->beginGroup("fileTree");
        ui->fileTree->setCurrentIndex(model->index(settings->value("path", "C:/").toString()));
        ui->fileTree->setColumnWidth(0, settings->value("nameWidth", 200).toInt());
        ui->fileTree->setColumnWidth(3, settings->value("dateWidth", 200).toInt());
        ui->fileMask->setText(settings->value("extension", ".txt").toString());
    settings->endGroup();
    settings->beginGroup("fileTable");
        ui->fileTable->setColumnWidth(0, settings->value("nameWidth", 200).toInt());
        ui->fileTable->setColumnWidth(1, settings->value("sampleWidth", 200).toInt());
        ui->fileTable->setColumnWidth(3, settings->value("commentWidth", 200).toInt());
        ui->fileTable->setColumnWidth(4, settings->value("dateWidth", 200).toInt());
        ui->fileTable->setColumnWidth(2, settings->value("elementWidth", 200).toInt());
    settings->endGroup();
    settings->beginGroup("chart");
        ui->chartWidget->resize(settings->value("lastSize", QSize(800, 600)).toSize());
        //ui->bSpinBox->setValue(settings->value("leftLength", 1).toDouble()); //dont know, why it fails if placed here
        //ui->tSpinBox->setValue(settings->value("rightLength", 1).toDouble()); // same
        //ui->dividerSpinBox->setValue(settings->value("dividerPos", 700). toDouble());
    settings->endGroup();
    settings->beginGroup("diffChart");
        ui->diffWidget->resize(settings->value("lastSize", QSize(800, 600)).toSize());
    settings->endGroup();
    settings->endGroup();
}

void MainWindow::saveSettings(){
    settings->beginGroup("gui");
    settings->setValue("windowSize", this->size());
    settings->setValue("windowPosition", this->pos());
    settings->setValue("windowTitle", this->windowTitle());
    settings->setValue("lastDrive", ui->driveBox->currentText());
    settings->beginGroup("fileTree");
        settings->setValue("path", model->filePath(ui->fileTree->currentIndex()));
        settings->setValue("nameWidth", ui->fileTree->columnWidth(0));
        settings->setValue("dateWidth", ui->fileTree->columnWidth(3));
        settings->setValue("extension", ui->fileMask->text());
    settings->endGroup();
    settings->beginGroup("fileTable");
        settings->setValue("nameWidth", ui->fileTable->columnWidth(0));
        settings->setValue("sampleWidth", ui->fileTable->columnWidth(1));
        settings->setValue("elementWidth", ui->fileTable->columnWidth(2));
        settings->setValue("dateWidth", ui->fileTable->columnWidth(4));
        settings->setValue("commentWidth", ui->fileTable->columnWidth(3));
    settings->endGroup();
    settings->beginGroup("chart");
        settings->setValue("lastSize", ui->chartWidget->size());
    settings->endGroup();
    settings->beginGroup("diffChart");
        settings->setValue("lastSize", ui->diffWidget->size());
    settings->endGroup();
    settings->endGroup();
    settings->beginGroup("external");
        settings->setValue("leftLength", ui->bSpinBox->value());
        settings->setValue("rightLength", ui->tSpinBox->value());
        settings->setValue("dividerPos", ui->dividerSpinBox->value());
    settings->endGroup();
}

void MainWindow::refreshButton(){
    refresh();
}

void MainWindow::refresh(){
    QString prev = ui->driveBox->currentText();
    if(prev.length() == 0){
        prev = drive;
    }
    ui->driveBox->clear();
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            ui->driveBox->addItem(storage.rootPath());
        }
    }
    int tmp = ui->driveBox->findText(prev);
    if(tmp != -1){
        ui->driveBox->setCurrentIndex(tmp);
    }
    buildFileTree();
}

void MainWindow::buildFileTree(){
    model->setRootPath(ui->driveBox->currentText());
    model->setNameFilters(QStringList("*" + ui->fileMask->text()));
    model->setNameFilterDisables(false);
    ui->fileTree->setModel(model);
    ui->fileTree->setRootIndex(model->index(ui->driveBox->currentText()));
    ui->fileTree->setIndentation(5);
    ui->fileTree->setColumnWidth(0, 200);
    ui->fileTree->setColumnHidden(1, true);
    ui->fileTree->setColumnHidden(2, true);
}

void MainWindow::driveChanged(int i){
    Q_UNUSED(i);
    buildFileTree();
}

void MainWindow::open(QModelIndex i){
    if(model->fileInfo(i).suffix().length() != 0){
        filePath = model->fileInfo(i).path();
        table->removeAll();
        QModelIndex parentIndex = i.parent();
        int numRows = model->rowCount(parentIndex);
        for (int row = 0; row < numRows; ++row) {
            table->insertRow(row, QModelIndex());
            QModelIndex childIndex = model->index(row, 0, parentIndex);
            QModelIndex index = table->index(row, 0, QModelIndex());
            table->setData(index, model->fileName(childIndex), Qt::EditRole);
            QString tmp = model->filePath(childIndex);
            tmp.chop(3);
            par = new QSettings(tmp + "par", QSettings::IniFormat);
            tmp = model->fileName(childIndex);
            tmp.chop(4);
            par->beginGroup(tmp);
            index = table->index(row, 1, QModelIndex());
            table->setData(index, par->value("sampleName", "no info") , Qt::EditRole);
            index = table->index(row, 2, QModelIndex());
            table->setData(index, par->value("rating", "no info"), Qt::EditRole);
            index = table->index(row, 3, QModelIndex());
            table->setData(index, par->value("comment", "no info"), Qt::EditRole);
            par->endGroup();
            index = table->index(row, 4, QModelIndex());
            table->setData(index, model->fileInfo(childIndex).created().toString("dd.MM.yyyy   HH:m"), Qt::EditRole);
        }
        ui->fileTable->selectRow(i.row());
        currentSelection = ui->fileTable->indexAt(QPoint(0, i.row()));

    }
}

void MainWindow::load(QModelIndex index){
    index = table->index(index.row(), 0);
    fileName = table->data(index, Qt::DisplayRole).toString();
    dataLoader = new FileLoader(filePath + "/" + fileName);
    ui->tabWidget->setCurrentIndex(1);
    settings->beginGroup("external");
        ui->bSpinBox->setValue(settings->value("leftLength", 1).toDouble());
        ui->tSpinBox->setValue(settings->value("rightLength", 1).toDouble());
        ui->dividerSpinBox->setValue(settings->value("dividerPos", 700). toDouble());
    settings->endGroup();
    reCalc();
    chart->legend()->hide();
    diff->legend()->hide();
}

void MainWindow::load(){
    load(currentSelection);
}

QModelIndex MainWindow::selected(QModelIndex index){
    return index;
}

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    chartView->resize(ui->chartWidget->size());
    diffView->resize(ui->diffWidget->size());
}

void MainWindow::reCalc(){
    qDebug() << "reCalc";
    if(ui->steppedBackgroundBox->isChecked()){
        //ui->maxBox->setChecked(true);
    }
    if(ui->rawBox->isChecked() || ui->zeroBox->isChecked()){
        ui->linearBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
    }
    dataLoader->setLimits(ui->bSpinBox->value(), ui->tSpinBox->value());
    data = dataLoader->getData();
    bareData = dataLoader->getBareData();
    if(chart->axes().contains(axisX)){
        chart->removeAxis(axisX);
    }
    if(chart->axes().contains(axisY)){
        chart->removeAxis(axisY);
    }
    if(diff->axes().contains(diffX)){
        diff->removeAxis(diffX);
    }
    if(diff->axes().contains(diffY)){
        diff->removeAxis(diffY);
    }
    axisX->setMin(data.first().first);
    axisX->setMax(data.last().first);
    diffX->setMin(data.first().first);
    diffX->setMax(data.last().first);
    if(ui->dividerSpinBox->value() < axisX->min() || ui->dividerSpinBox->value() > axisX->max()){
        ui->dividerSpinBox->setValue(axisX->min() + (axisX->max() - axisX->min())/2);
    }
    if(chart->series().contains(lNorm)){
        chart->removeSeries(lNorm);
    }
    if(chart->series().contains(rNorm)){
        chart->removeSeries(rNorm);
    }
    if(chart->series().contains(l)){
        chart->removeSeries(l);
    }
    if(chart->series().contains(r)){
        chart->removeSeries(r);
    }
    qreal min = 1;
    qreal max = -1;
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    diff->addAxis(diffX, Qt::AlignBottom);
    diff->addAxis(diffY, Qt::AlignLeft);
    QPair<qreal, QPair<qreal, qreal>> pair;
    if(bareData.length() != 0 && ui->rawBox->checkState() == 2){
        r = new QtCharts::QLineSeries();
        l = new QtCharts::QLineSeries();
        int j = 1;
        foreach(pair, bareData){
            l->append(pair.first, pair.second.first);
            r->append(pair.first, pair.second.second);
            if(min > pair.second.first){
                min = pair.second.first;
            }
            if(max < pair.second.first){
                max = pair.second.first;
            }
            if(min > pair.second.second){
                min = pair.second.second;
            }
            if(max < pair.second.second){
                max = pair.second.second;
            }
            j++;
        }
        r->setColor(QColor(255,0 , 0));
        l->setColor(QColor(0, 255, 0));
        chart->addSeries(r);
        chart->addSeries(l);
        l->attachAxis(axisY);
        r->attachAxis(axisY);
    }
    if(chart->series().contains(ll)){
        chart->removeSeries(ll);
    }
    if(chart->series().contains(rl)){
        chart->removeSeries(rl);
    }
    if(chart->series().contains(lIntervals)){
        chart->removeSeries(lIntervals);
    }
    if(ui->linearBox->isChecked() || ui->linearBackgroundBox->isChecked()){
        lIntervals = new QtCharts::QLineSeries();
        lIntervals->append(data.first().first + ui->llSpinBox->value(), axisY->max());
        lIntervals->append(data.first().first + ui->llSpinBox->value(), axisY->min() - 1);
        lIntervals->append(data.last().first - ui->rlSpinBox->value(), axisY->min() - 1);
        lIntervals->append(data.last().first - ui->rlSpinBox->value(), axisY->max());
        chart->addSeries(lIntervals);
        lIntervals->attachAxis(axisX);
        lIntervals->attachAxis(axisY);
        qreal xlSum = 0;
        qreal ylSum = 0;
        qreal xxlSum = 0;
        qreal xylSum = 0;
        qreal xrSum = 0;
        qreal yrSum = 0;
        qreal xxrSum = 0;
        qreal xyrSum = 0;
        int ln = 0;
        int rn = 0;
        foreach(pair, data){
            if(ui->llSpinBox->value() > 0 && pair.first < data.first().first + ui->llSpinBox->value()){
                ln++;
                xlSum += pair.first;
                ylSum += (pair.second.first + pair.second.second)/2;
                xxlSum += pow(pair.first, 2);
                xylSum += pair.first*(pair.second.first + pair.second.second)/2;
            }
            if(ui->rlSpinBox->value() > 0 && pair.first > data.last().first - ui->rlSpinBox->value()){
                rn++;
                xrSum += pair.first;
                yrSum += (pair.second.first + pair.second.second)/2;
                xxrSum += pow(pair.first, 2);
                xyrSum += pair.first*(pair.second.first + pair.second.second)/2;
            }
        }
        qreal al;
        qreal bl;
        qreal ar;
        qreal br;
        if(ln > 1){
            al = (ln*xylSum - xlSum*ylSum)/(ln*xxlSum - pow(xlSum, 2));
            bl = (ylSum - al*xlSum)/ln;
            ll = new QtCharts::QLineSeries();
            a = al;
            b = bl;
        }
        if(rn > 1){
            ar = (rn*xyrSum - xrSum*yrSum)/(rn*xxrSum - pow(xrSum, 2));
            br = (yrSum - ar*xrSum)/rn;
            rl = new QtCharts::QLineSeries();
        }
        for(int i = 0; i < data.length(); i++){
            if(ln > 1 && !ui->linearBackgroundBox->isChecked()){
                ll->append(data.at(i).first, al*data.at(i).first + bl);
                if(min > al*data.at(i).first + bl){
                    min = al*data.at(i).first + bl;
                }
            }
            if(rn > 1){
                if(!ui->linearBackgroundBox->isChecked()){
                    rl->append(data.at(i).first, ar*data.at(i).first + br);
                }else{
                    rl->append(data.at(i).first, (ar - al)*data.at(i).first + br - bl);
                }
            }
        }
        if(ln > 1){
            chart->addSeries(ll);
            ll->attachAxis(axisY);
            ui->lkLabel->setText(QString::number(al));
        }
        if(rn > 1){
            chart->addSeries(rl);
            rl->attachAxis(axisY);
            ui->rkLabel->setText(QString::number(ar));
        }
    }
    if(ui->linearBackgroundBox->isChecked()){
        ui->normBox->setChecked(true);
        for(int c = 0; c < data.length(); c++){
            data.replace(c, QPair<qreal, QPair<qreal, qreal>>(data.at(c).first, QPair<qreal, qreal>(data.at(c).second.first - a*data.at(c).first - b, data.at(c).second.second - a*data.at(c).first - b)));
        }
    }
    if(chart->series().contains(stepsSeries)){
        chart->removeSeries(stepsSeries);
    }
    qreal mult = 1;
    qreal step = 0;
    if(ui->stepBox->isChecked()){
        step = data.first().second.first - data.first().second.second;
    }
    if(ui->mulBox->isChecked()){
        mult = data.first().second.first/data.first().second.second;
    }
    if(ui->mulBox->isChecked() || ui->stepBox->isChecked()){
        for(int c = 0; c < data.length(); c++){
            data.replace(c, QPair<qreal, QPair<qreal, qreal>>(data.at(c).first, QPair<qreal, qreal>(data.at(c).second.first, data.at(c).second.second * mult + step)));
        }
    }
    if(ui->steppedBackgroundBox->isChecked()){
        ui->linearBackgroundBox->setChecked(true);
        stepsSeries = new QtCharts::QLineSeries();
        qreal m1 = ((data.last().second.first - data.first().second.first)*2/3)/(2*qAcos(0));
        qreal m2 = ((data.last().second.first - data.first().second.first)/3)/(2*qAcos(0));
        for(int i = 0; i < data.length(); i++){
            qreal tmp = data.first().second.first + m1 * (qAcos(0) + qAtan(ui->filletSpinBox->value()*(data.at(i).first - data.at(lInd.first).first))) + m2 * (qAcos(0) + qAtan(ui->filletSpinBox->value()*(data.at(i).first - data.at(lInd.second).first)));
            stepsSeries->append(data.at(i).first, tmp);
        }
        chart->addSeries(stepsSeries);
        stepsSeries->attachAxis(axisY);
        stepsSeries->attachAxis(axisX);
    }
    if(data.length() != 0 && ui->normBox->checkState() == 2){
        lNorm = new QtCharts::QLineSeries();
        rNorm = new QtCharts::QLineSeries();
        foreach(pair, data){
            rNorm->append(pair.first, pair.second.second);
            lNorm->append(pair.first, pair.second.first);
            if(min > pair.second.second){
                min = pair.second.second;
            }
            if(max < pair.second.second){
                max = pair.second.second;
            }
            if(min > pair.second.first){
                min = pair.second.first;
            }
            if(max < pair.second.first){
                max = pair.second.first;
            }
        }
        chart->addSeries(rNorm);
        chart->addSeries(lNorm);
        rNorm->setColor(QColor(200,0 , 0));
        lNorm->setColor(QColor(0, 200, 0));
        lNorm->attachAxis(axisY);
        rNorm->attachAxis(axisY);
    }
    if(chart->series().contains(lz)){
        chart->removeSeries(lz);
    }
    if(chart->series().contains(rz)){
        chart->removeSeries(rz);
    }
    if(data.length() != 0 && ui->zeroBox->checkState() == 2){
        lz = new QtCharts::QLineSeries();
        rz = new QtCharts::QLineSeries();
        QVector<QPair<qreal,QPair<qreal, qreal>>> zeroData = dataLoader->getZero();
        QPair<qreal, QPair<qreal, qreal>> tmp;
        foreach(tmp, zeroData){
            lz->append(tmp.first, tmp.second.first);
            rz->append(tmp.first, tmp.second.second);
            if(min > tmp.second.first){
                min = tmp.second.first;
            }else{
                if(max < tmp.second.first){
                    max = tmp.second.first;
                }
            }
            if(min > tmp.second.second){
                min = tmp.second.second;
            }else{
                if(max < tmp.second.second){
                    max = tmp.second.second;
                }
            }
        }
        rz->setColor(QColor(200,0 , 0));
        lz->setColor(QColor(0, 200, 0));
        chart->addSeries(lz);
        lz->attachAxis(axisY);
        lz->attachAxis(axisX);
        chart->addSeries(rz);
        rz->attachAxis(axisY);
        rz->attachAxis(axisX);
    }
    axisX->setMinorTickCount(4);
    axisX->setTickCount(20);
    axisX->setTitleText("primary photons energy, eV");
    diffX->setMinorTickCount(4);
    diffX->setTickCount(20);
    diffX->setTitleText("primary photons energy, eV");
    axisY->setMin(min);
    axisY->setMax(max);
    axisY->setMinorTickCount(2);
    axisY->setTickCount(10);
    axisY->setTitleText("Intensity");
    findMax();
    if(chart->series().contains(lMax)){
        chart->removeSeries(lMax);
    }
    if(chart->series().contains(rMax)){
        chart->removeSeries(rMax);
    }
    rMax = new QtCharts::QLineSeries();
    lMax = new QtCharts::QLineSeries();
    if(ui->maxBox->isChecked()){
        lMax->append(data.at(lInd.first).first, data.at(lInd.first).second.first);
        lMax->append(data.at(lInd.first).first, axisY->min());
        lMax->append(data.at(lInd.second).first, axisY->min());
        lMax->append(data.at(lInd.second).first, data.at(lInd.second).second.first);
        chart->addSeries(lMax);
        lMax->attachAxis(axisY);
        lMax->attachAxis(axisX);
        rMax->append(data.at(rInd.first).first, data.at(rInd.first).second.second);
        rMax->append(data.at(rInd.first).first, axisY->min());
        rMax->append(data.at(rInd.second).first, axisY->min());
        rMax->append(data.at(rInd.second).first, data.at(rInd.second).second.second);
        chart->addSeries(rMax);
        rMax->attachAxis(axisY);
        rMax->attachAxis(axisX);
    }
    rMax->setColor(QColor(200,0 , 0));
    lMax->setColor(QColor(0, 200, 0));
    if(chart->series().contains(divider)){
        chart->removeSeries(divider);
    }
    divider = new QtCharts::QLineSeries();
    if(ui->dividerBox->isChecked()){
        divider->append(ui->dividerSpinBox->value(), axisY->min());
        divider->append(ui->dividerSpinBox->value(), axisY->max());
        chart->addSeries(divider);
        divider->attachAxis(axisY);
        divider->attachAxis(axisX);
    }
    if(diff->series().contains(diffS)){
        diff->removeSeries(diffS);
    }
    if(diff->series().contains(l0)){
        diff->removeSeries(l0);
    }
    diffY->setMinorTickCount(2);
    diffY->setTickCount(10);
    diffY->setTitleText("Intensity");
    diffS = new QtCharts::QLineSeries();
    l0 = new QtCharts::QLineSeries();
    min = 1;
    max = -1;
    if(ui->diffBox->isChecked()){
        qreal tmp;
        foreach (pair, data) {
            tmp = pair.second.second - pair.second.first;
            if(tmp > max){
                max = tmp;
            }else{
                if(tmp < min){
                    min = tmp;
                }
            }
            diffS->append(pair.first, tmp);
            l0->append(pair.first, 0);
        }
        diffY->setMin(min);
        diffY->setMax(max);
        diff->addSeries(diffS);
        diff->addSeries(l0);
        l0->attachAxis(diffY);
        l0->attachAxis(diffX);
        diffS->attachAxis(diffY);
        diffS->attachAxis(diffX);
    }
    if(ui->integrateBox->isChecked()){
        if(!ui->steppedBackgroundBox->isChecked() || !ui->linearBackgroundBox->isChecked() || !ui->diffBox->isChecked()){
            ui->integrateBox->setChecked(false);
        }else{
            qreal summ = 0;
            for(int i = 0; i < data.length() - 1; i++){
                summ += (data.at(i + 1).first - data.at(i).first)*(data.at(i + 1).second.first + data.at(i).second.first)/2;
                summ += (data.at(i + 1).first - data.at(i).first)*(data.at(i + 1).second.second + data.at(i).second.second)/2;
                summ -= (data.at(i + 1).first - data.at(i).first)*(stepsSeries->at(i).y() + stepsSeries->at(i + 1).y())/2;
            }
            ui->sumLabel->setText(QString::number(summ));
            qreal localMin = 1;
            int localMinInd = 0;
            for(int i = lInd.first; i < lInd.second; i++){
                if((data.at(i).second.first + data.at(i).second.second)/2 < localMin){
                   localMin = (data.at(i).second.first + data.at(i).second.second)/2;
                   localMinInd = i;
                }
            }
            qreal dl3 = 0;
            for(int i = 0; i < localMinInd - 1; i++){
                dl3 += (diffS->at(i + 1).x() - diffS->at(i).x())*(diffS->at(i + 1).y() + diffS->at(i).y())/2;
            }
            ui->l3Label->setText(QString::number(dl3));
            qreal dl2 = 0;
            for(int i = localMinInd; i < data.length() - 1; i++){
                dl2 += (diffS->at(i + 1).x() - diffS->at(i).x())*(diffS->at(i + 1).y() + diffS->at(i).y())/2;
            }
            ui->l2Label->setText(QString::number(dl2));
            msEff = -2*(Nh*muB/Pc)*(dl3 - 2*dl2)/summ;
            ui->msEffLabel->setText(QString::number(msEff));
            mOrb = -(4/3)*(Nh*muB/Pc)*(dl3 + dl2)/summ;
            ui->mOrbLabel->setText(QString::number(mOrb));
        }
    }else{
        if(!(ui->sumLabel->text() == "Unknown")){
            qDebug() << "old";
            ui->sumLabel->setText(ui->sumLabel->text() + " OLD!");
            ui->l3Label->setText(ui->l3Label->text() + " OLD!");
            ui->l2Label->setText(ui->l2Label->text() + " OLD!");
            ui->msEffLabel->setText(ui->msEffLabel->text() + " OLD!");
            ui->mOrbLabel->setText(ui->mOrbLabel->text() + " OLD!");
            ui->msLabel->setText(ui->msLabel->text() + " OLD!");
            ui->mTLabel->setText(ui->mTLabel->text() + " OLD!");
            ui->mOrbOLabel->setText(ui->mOrbOLabel->text() + " OLD!");
            ui->mOrbPLabel->setText(ui->mOrbPLabel->text() + " OLD!");
        }
    }
}

void MainWindow::exportCharts(){
    outFile = new QFile(filePath + "/" + "OUTPUT_" + fileName);
    outFile->open(QIODevice::WriteOnly);
    outStream = new QTextStream(outFile);
    *outStream << "processing params :" << "\n";
    *outStream << "lChopLength = " << ui->bSpinBox->value() << "\n";
    *outStream << "rChopLength = " << ui->tSpinBox->value() << "\n";
    *outStream << "normalisation type = ";
    if(ui->stepBox->isChecked()){
        *outStream << "i-zero + constant step";
    }else{
        if(ui->mulBox->isChecked()){
            *outStream << "i-zero * constant";
        }else{
            *outStream << "none";
        }
    }
    *outStream << "\n";
    *outStream << "leftLinearisationInterval = " << ui->llSpinBox->value() << "\n";
    *outStream << "rightLinearisationInterval = " << ui->rlSpinBox->value() << "\n";
    *outStream << "dividerPos = " << ui->dividerSpinBox->value() << "\n" ;
    *outStream << "linear coeff : l = " << ui->lkLabel->text() << " r = " << ui->rkLabel->text() << "\n";
    *outStream << "fillet mult = " << ui->filletSpinBox->value() << "\n\n";
    *outStream << "legend:" << "\n" ;
    *outStream << "x = wavelength (column 2 in original file)" << "\n" ;
    *outStream << "l = l bare data (column 3 in original file)" << "\n" ;
    *outStream << "r = r bare data (column 7 in original file)" << "\n" ;
    *outStream << "lN = normalised, maybe stepped down data (column 6 in original file)" << "\n" ;
    *outStream << "rN = normalised, maybe stepped down data (column 10 in original file)" << "\n" ;
    *outStream << "l Max = l maximums" << "\n" ;
    *outStream << "r Max = r maximums" << "\n" ;
    *outStream << "diff = rN - lN"  << "\n\n";
    *outStream << "results:" << "\n";
    *outStream << "Al3 + Al2 = " << ui->sumLabel->text() << "\n";
    *outStream << "dAl3 = " << ui->l3Label->text() << "\n";
    *outStream << "dAl2 = " << ui->l2Label->text() << "\n";
    *outStream << "msEff = " << msEff << "\n";
    *outStream << "mOrb = " << mOrb << "\n";
    *outStream << "ms = " << ui->msLabel->text() << "\n";
    *outStream << "mT = " << ui->mTLabel->text() << "\n";
    *outStream << "mOrbOrt = " << ui->mOrbOLabel->text() << "\n";
    *outStream << "mOrbPar = " << ui->mOrbPLabel->text() << "\n";

    *outStream << "#";
    *outStream << qSetFieldWidth(9) << center << "x";
    *outStream << qSetFieldWidth(14) << left;
    if(ui->rawBox->isChecked()){
        *outStream << "l";
        *outStream << "r";
    }
    if(ui->normBox->isChecked()){
        *outStream << "lN";
        *outStream << "rN";
    }
    if(ui->maxBox->isChecked()){
        *outStream << "l Max";
        *outStream << "r Max";
    }
    if(ui->diffBox->isChecked()){
        *outStream << "diff";
    }
    *outStream << qSetFieldWidth(1) << "\n";

    for(int i = 0; i < data.length(); i++){
        *outStream  << qSetFieldWidth(10) << left << data.at(i).first;
        *outStream << qSetFieldWidth(14) << left;
        if(ui->rawBox->isChecked()){
            *outStream << bareData.at(i).second.first;
            *outStream << bareData.at(i).second.second;
        }
        if(ui->normBox->isChecked()){
            *outStream << data.at(i).second.first;
            *outStream << data.at(i).second.second;
        }
        if(ui->maxBox->isChecked()){
            if(lInd.first == i || lInd.second == i){
                *outStream << data.at(i).second.first;
            }else{
                *outStream << 0;
            }
            if(rInd.first == i || rInd.second == i){
                *outStream << data.at(i).second.second;
            }else{
                *outStream << 0;
            }
        }
        if(ui->diffBox->isChecked()){
            *outStream << (data.at(i).second.first - data.at(i).second.second);
        }
        *outStream << qSetFieldWidth(1) << "\n";
    }
    *outStream << qSetFieldWidth(4) << "\nEOF";
    outFile->close();
}

void MainWindow::findMax(){
    if(ui->zeroBox->isChecked()){
        ui->maxBox->setChecked(false);
    }else{
        qreal lmin = axisY->min();
        qreal rmin = lmin;
        bool cross = false;
        QPair<qreal, QPair<qreal, qreal>> pair;
        int i = 0;
        foreach(pair, data){
            if(pair.first > ui->dividerSpinBox->value() && !cross){
                cross = true;
                lmin = axisY->min();
                rmin = lmin;
            }
            if(pair.second.first > lmin){
                lmin = pair.second.first;
                if(cross){
                    lInd.second = i;
                }else{
                    lInd.first = i;
                }
            }
            if(pair.second.second > rmin){
                rmin = pair.second.second;
                if(cross){
                    rInd.second = i;
                }else{
                    rInd.first = i;
                }
            }
            i++;
        }
    }
}

void MainWindow::fullNorm(){
    if(ui->stepBox->isChecked() || ui->mulBox->isChecked()){
        ui->normBox->setChecked(true);
    }
    if(mul && ui->stepBox->isChecked()){
        mul = false;
        ui->mulBox->setChecked(mul);
    }else{
        mul = true;
        ui->stepBox->setChecked(false);
    }
    reCalc();
}

void MainWindow::lChanged(){
    if(!ui->linearBackgroundBox->isChecked()){
        ui->lkLabel->setText("Unknown");
        ui->rkLabel->setText("Unknown");
        ui->sumLabel->setText("Unknown");
        ui->l3Label->setText("Unknown");
        ui->l2Label->setText("Unknown");
    }
    reCalc();
}

void MainWindow::bg(){
    if((ui->linearBackgroundBox->isChecked() || ui->steppedBackgroundBox->isChecked()) && !ui->normBox->isChecked()){
        ui->normBox->setChecked(true);
    }
    reCalc();
}
