#include "mainwindow.h"
#include <complex>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    /*To-do list:
        * what data use to calculate angle? Currently program uses modyfied data and it works bad.
        * carefully delete everything in destructor
    */

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
    state = new QHash<QString,qreal>();
    QObject::connect(ui->refreshButton, &QPushButton::pressed, this, &MainWindow::refreshButton);
    QObject::connect(ui->driveBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::driveChanged);
    QObject::connect(ui->fileMask, &QLineEdit::editingFinished, this, &MainWindow::refresh);
    QObject::connect(ui->fileTree, &QTreeView::clicked, this, &MainWindow::open);
    QObject::connect(ui->fileTable, &QTableView::doubleClicked, this, static_cast<void(MainWindow::*)(QModelIndex)>(&MainWindow::load));
    QObject::connect(ui->fileTable, &QTableView::clicked, this, &MainWindow::selected);
    QObject::connect(ui->llSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->rlSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->bSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->tSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->dividerSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::lChanged);
    QObject::connect(ui->bSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->tSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->dividerSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->exportButton, &QPushButton::pressed, this, &MainWindow::exportCharts);
    QObject::connect(ui->elseGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->normGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->dividerBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    QObject::connect(ui->llSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->rlSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->backgroundGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::bg);
    QObject::connect(ui->backgroundGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->filletSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->fileGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::fileSelect);
    QObject::connect(ui->forget1Button, &QPushButton::pressed, this, &MainWindow::forget1);
    QObject::connect(ui->forget2Button, &QPushButton::pressed, this, &MainWindow::forget2);
    QObject::connect(ui->pSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->nSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->unitBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::reCalcBoth);
    QObject::connect(ui->filterGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reopen);
    QObject::connect(ui->angleGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reCalc);
    QObject::connect(ui->levelSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->tabWidget, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), this, &MainWindow::myResize);
    QObject::connect(ui->levelBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    QObject::connect(ui->swapButton, &QPushButton::pressed, this, &MainWindow::swap);
    buildFileTree();
    loadSettings();
    refresh();
}

MainWindow::~MainWindow()// recheck the system
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
    delete state;
    delete ui;
    delete par;
    delete settings;
}

void MainWindow::myResize(){
    chartView->resize(ui->chartWidget->size());
    diffView->resize(ui->diffWidget->size());
}

void MainWindow::swap(){
    if(ui->file1Box->isChecked()){
        ui->file2Box->setChecked(true);
    }else{
        ui->file1Box->setChecked(true);
    }
    fileSelect();
}

void MainWindow::loadSettings(){
    settings->beginGroup("gui");
    resize(settings->value("windowSize", QSize(800, 600)).toSize());
    move(settings->value("windowPosition", QPoint(0, 0)).toPoint());
    setWindowTitle(settings->value("windowTitle", "XMCDAnalyzer(corrupted .ini)").toString());
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
        ui->fileTable->setColumnWidth(2, settings->value("geometryWidth", 200).toInt());
        ui->fileTable->setColumnWidth(4, settings->value("commentWidth", 200).toInt());
        ui->fileTable->setColumnWidth(5, settings->value("dateWidth", 200).toInt());
        ui->fileTable->setColumnWidth(3, settings->value("elementWidth", 200).toInt());
    settings->endGroup();
    settings->beginGroup("chart");
        ui->chartWidget->resize(settings->value("lastSize", QSize(800, 600)).toSize());
        //ui->bSpinBox->setValue(settings->value("leftLength", 1).toDouble()); //dont know, why it fails if placed here
        //ui->tSpinBox->setValue(settings->value("rightLength", 1).toDouble()); // same
        //ui->dividerSpinBox->setValue(settings->value("dividerPos", 700). toDouble());
    settings->endGroup();
    settings->beginGroup("diffChart");
        if(settings->value("angle1", true).toBool()){
            ui->angle1Box->setChecked(true);
        }else{
            ui->angle2Box->setChecked(true);
        }
        ui->diffWidget->resize(settings->value("lastSize", QSize(800, 600)).toSize());
    settings->endGroup();
    settings->endGroup();
    ui->levelSpinBox->setEnabled(false);
    ui->levelBox->setEnabled(false);
    myResize();
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
        settings->setValue("geometryWidth", ui->fileTable->columnWidth(2));
        settings->setValue("elementWidth", ui->fileTable->columnWidth(3));
        settings->setValue("dateWidth", ui->fileTable->columnWidth(5));
        settings->setValue("commentWidth", ui->fileTable->columnWidth(4));
    settings->endGroup();
    settings->beginGroup("chart");
        settings->setValue("lastSize", ui->chartWidget->size());
    settings->endGroup();
    settings->beginGroup("diffChart");
        settings->setValue("angle1", ui->angle1Box->isChecked());
        settings->setValue("lastSize", ui->diffWidget->size());
    settings->endGroup();
    settings->endGroup();
}

void MainWindow::reopen(){
    open(ui->fileTree->currentIndex());
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
        if(ui->file1Box->isChecked()){
            file1Path = model->fileInfo(i).path();
            file2Path = model->fileInfo(i).path();
        }
        table->removeAll();
        QModelIndex parentIndex = i.parent();
        int numRows = model->rowCount(parentIndex);
        int row = 0;
        for (int i = 0; i < numRows; i++) {
            QModelIndex childIndex = model->index(i, 0, parentIndex);
            QString tmp = model->filePath(childIndex);
            QFile* currentFile = new QFile(tmp);
            currentFile->open(QIODevice::ReadOnly);
            QString line = QTextStream(currentFile).readLine();
            currentFile->close();
            bool sameEnergy = true;
            if(ui->energyBox->isChecked() && (energy != line.split("	").first())){
                sameEnergy = false;
            }
            tmp.chop(3);
            par = new QSettings(tmp + "par", QSettings::IniFormat);
            tmp = model->fileName(childIndex);
            tmp.chop(4);
            par->beginGroup(tmp);
            bool sameSample = true;
            if(ui->sampleBox->isChecked() && (par->value("sampleName", "no info").toString() != sample)){
                sameSample = false;
            }
            bool sameGeom = true;
            if(ui->geomBox->isChecked() && ((par->value("angle", "-1").toString() == "-1") || (par->value("angle", "-1").toString() == "") || (par->value("angle", "noInfo").toString() == geom))){
                sameGeom = false;
            }
            if(((loaded1) && sameSample && sameGeom && sameEnergy) || ui->file1Box->isChecked()){
                table->insertRow(row, QModelIndex());
                QModelIndex index = table->index(row, 0, QModelIndex());
                table->setData(index, model->fileName(childIndex), Qt::EditRole);
                index = table->index(row, 1, QModelIndex());
                table->setData(index, par->value("sampleName", "no info") , Qt::EditRole);
                index = table->index(row, 2, QModelIndex());
                tmp = par->value("angle", "-1").toString();
                if(tmp == ""){
                    tmp = "-1";
                }
                table->setData(index, tmp , Qt::EditRole);
                index = table->index(row, 3, QModelIndex());
                table->setData(index, par->value("rating", "no info"), Qt::EditRole);
                index = table->index(row, 4, QModelIndex());
                table->setData(index, par->value("comment", "no info"), Qt::EditRole);
                index = table->index(row, 5, QModelIndex());
                table->setData(index, model->fileInfo(childIndex).lastModified().toString("dd.MM.yyyy   HH:m"), Qt::EditRole);
                row++;
            }
            par->endGroup();
        }
        ui->fileTable->selectRow(i.row());
        currentSelection = ui->fileTable->indexAt(QPoint(0, i.row()));
    }
}

void MainWindow::load(QModelIndex index){
    index = table->index(index.row(), 1);
    sample = table->data(index, Qt::DisplayRole).toString();
    ui->exportLine->setText(file1Path + "/" + sample + ".txt");
    if(ui->file1Box->isChecked()){
        index = table->index(index.row(), 2);
        geom = table->data(index, Qt::DisplayRole).toString();
        index = table->index(index.row(), 0);
        file1Name = table->data(index, Qt::DisplayRole).toString();
        ui->file1Label->setText(file1Name);
        QFile* currentFile = new QFile(file1Path + "/" + file1Name);
        currentFile->open(QIODevice::ReadOnly);
        stream = new QTextStream(currentFile);
        QString line = stream->readLine();
        currentFile->close();
        energy = line.split("	").first();
        //insert wrong file format check here
        data1Loader = new FileLoader(file1Path + "/" + file1Name);
        index = table->index(index.row(), 2);
        teta1 = table->data(index, Qt::DisplayRole).toDouble();
        int t = teta1;
        switch (t) {
            case 0:
                teta1 = angle0;
                break;
            case 1:
                teta1 = angle1;
                break;
            case 2:
                teta1 = angle2;
                break;
        }
        teta1 = teta1*M_PI/180;
        ui->calculateBox->setChecked(false);
        ui->integrateBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->maxBox->setChecked(false);
        ui->linearBox->setChecked(false);
        ui->dividerBox->setChecked(false);
        ui->normBox->setChecked(true);
        ui->llSpinBox->setValue(0);
        ui->rlSpinBox->setValue(0);
        ui->bSpinBox->setValue(0);
        ui->tSpinBox->setValue(0);
        ui->filletSpinBox->setValue(10);
        loaded1 = true;
    }else{
        index = table->index(index.row(), 0);
        file2Name = table->data(index, Qt::DisplayRole).toString();
        ui->file2Label->setText(file2Name);
        data2Loader = new FileLoader(file2Path + "/" + file2Name);
        index = table->index(index.row(), 2);
        teta2 = table->data(index, Qt::DisplayRole).toDouble();
        int t = teta2;
        switch (t) {
            case 0:
                teta2 = angle0;
                break;
            case 1:
                teta2 = angle1;
                break;
            case 2:
                teta2 = angle2;
                break;
        }
        teta2 = teta2*M_PI/180;
        ui->calculateBox->setChecked(false);
        ui->integrateBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->maxBox->setChecked(false);
        ui->linearBox->setChecked(false);
        ui->dividerBox->setChecked(false);
        ui->normBox->setChecked(true);
        ui->llSpinBox->setValue(0);
        ui->rlSpinBox->setValue(0);
        ui->bSpinBox->setValue(0);
        ui->tSpinBox->setValue(0);
        ui->filletSpinBox->setValue(10);
        loaded2 = true;
    }
    reCalc();
    lLinearPrev = ui->llSpinBox->value();
    rLinearPrev = ui->rlSpinBox->value();
    dividerPrev = ui->dividerSpinBox->value();
    lChopPrev = ui->bSpinBox->value();
    rChopPrev = ui->tSpinBox->value();
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

void MainWindow::reCalc(){ //almost everything calculates here
    if((ui->file1Box->isChecked() && loaded1) || (ui->file2Box->isChecked() && loaded2)){
        if(ui->rawBox->isChecked() || ui->zeroBox->isChecked()){
        ui->linearBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
    }
    if(ui->file1Box->isChecked()){
        data1Loader->setLimits(ui->bSpinBox->value(), ui->tSpinBox->value());
        data = data1Loader->getData();
        bareData = data1Loader->getBareData();
    }else{
        data2Loader->setLimits(ui->bSpinBox->value(), ui->tSpinBox->value());
        data = data2Loader->getData();
        bareData = data2Loader->getBareData();
    }
    ui->llSpinBox->setMinimum(data.at(2).first - data.first().first);
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
    qreal min = 100000000;
    qreal max = -1;
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
    qreal mult = 1;
    qreal step1 = 0;
    qreal step2 = 0;
    qreal commonMult = 1;
    if(ui->stepBox->isChecked()){
        step2 = data.first().second.first - data.first().second.second;
    }else{
        if(ui->mulBox->isChecked()){
            mult = data.first().second.first/data.first().second.second;
        }else{
            if(ui->oneBox->isChecked()){
                foreach(pair, data){
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
                }
                commonMult = 100/(max - min);
                step1 = -data.first().second.first*commonMult;
                step2 = -data.first().second.second*commonMult;
            }
        }
    }
    if(ui->mulBox->isChecked() || ui->stepBox->isChecked() || ui->oneBox->isChecked()){
        for(int c = 0; c < data.length(); c++){
            data.replace(c, QPair<qreal, QPair<qreal, qreal>>(data.at(c).first, QPair<qreal, qreal>(data.at(c).second.first*commonMult + step1, data.at(c).second.second * mult*commonMult + step2)));
        }
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
        for(int c = 0; c < data.length(); c++){
            data.replace(c, QPair<qreal, QPair<qreal, qreal>>(data.at(c).first, QPair<qreal, qreal>(data.at(c).second.first - a*data.at(c).first - b, data.at(c).second.second - a*data.at(c).first - b)));
        }
    }
    if(chart->series().contains(stepsSeries)){
        chart->removeSeries(stepsSeries);
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
    if(data.length() != 0 && !(ui->rawBox->isChecked() || ui->zeroBox->isChecked())){
        lNorm = new QtCharts::QLineSeries();
        rNorm = new QtCharts::QLineSeries();
        if(ui->file1Box->isChecked()){
            limits1 = QPair<qreal, qreal>(data.first().first, data.last().first);
            tmp1Data = data;
        }else{
            limits2 = QPair<qreal, qreal>(data.first().first, data.last().first);
            tmp2Data = data;
        }
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
        QVector<QPair<qreal,QPair<qreal, qreal>>> zeroData;
        if(ui->file1Box->isChecked()){
            zeroData = data1Loader->getZero();
        }else{
            zeroData = data2Loader->getZero();
        }
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
            qreal localMin = 10000000000;
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
            qreal dl2 = 0;
            for(int i = localMinInd; i < data.length() - 1; i++){
                dl2 += (diffS->at(i + 1).x() - diffS->at(i).x())*(diffS->at(i + 1).y() + diffS->at(i).y())/2;
            }
            if(ui->file1Box->isChecked()){
                msEff1 = -2*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 - 2*dl2)/summ;
                mOrb1 = -(4/3)*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 + dl2)/summ;
                ui->l31Label->setText(QString::number(dl3));
                ui->sum1Label->setText(QString::number(summ));
                ui->l21Label->setText(QString::number(dl2));
                ui->msEff1Label->setText(QString::number(msEff1) + " " + units[ui->unitBox->currentIndex()]);
                ui->mOrb1Label->setText(QString::number(mOrb1) + " " + units[ui->unitBox->currentIndex()]);
                calc1 = true;
            }else{
                msEff2 = -2*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 - 2*dl2)/summ;
                mOrb2 = -(4/3)*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 + dl2)/summ;
                ui->l32Label->setText(QString::number(dl3));
                ui->sum2Label->setText(QString::number(summ));
                ui->l22Label->setText(QString::number(dl2));
                ui->msEff2Label->setText(QString::number(msEff2) + " " + units[ui->unitBox->currentIndex()]);
                ui->mOrb2Label->setText(QString::number(mOrb2) + " " + units[ui->unitBox->currentIndex()]);
                calc2 = true;
            }
        }
    }else{
        ui->calculateBox->setChecked(false);
        if(ui->file1Box->isChecked()){
            ui->sum1Label->setText("Unknown");
            ui->l31Label->setText("Unknown");
            ui->l21Label->setText("Unknown");
            ui->msEff1Label->setText("Unknown");
            ui->mOrb1Label->setText("Unknown");
            ui->msLabel->setText("Unknown");
            ui->mTLabel->setText("Unknown");
            ui->mOrbOLabel->setText("Unknown");
            ui->mOrbPLabel->setText("Unknown");
            calc1 = false;
        }else{
            ui->sum2Label->setText("Unknown");
            ui->l32Label->setText("Unknown");
            ui->l22Label->setText("Unknown");
            ui->msEff2Label->setText("Unknown");
            ui->mOrb2Label->setText("Unknown");
            ui->msLabel->setText("Unknown");
            ui->mTLabel->setText("Unknown");
            ui->mOrbOLabel->setText("Unknown");
            ui->mOrbPLabel->setText("Unknown");
            calc2 = false;
        }
    }
    if(chart->series().contains(level)){
        chart->removeSeries(level);
    }
    if(loaded1 && loaded2){
        if(ui->levelSpinBox->value() < limits1.first - eps || ui->levelSpinBox->value() < limits2.first - eps || ui->levelSpinBox->value() > limits1.second || ui->levelSpinBox->value() > limits2.second){
            ui->levelSpinBox->setValue(qMin(limits1.second, limits2.second));
        }
        ui->levelSpinBox->setMaximum(qMin(limits1.second, limits2.second));
        ui->levelSpinBox->setMinimum(qMax(limits1.first, limits2.first));
        int i1 = 0;
        int i2 = 0;
        while(tmp1Data.at(i1).first < ui->levelSpinBox->value()){
            i1++;
        }
        while(tmp2Data.at(i2).first < ui->levelSpinBox->value()){
            i2++;
        }
        QPair<qreal, qreal> intens = QPair<qreal, qreal>((data1Loader->getData().at(i1).second.first + data1Loader->getData().at(i1).second.second)/2, (data2Loader->getData().at(i2).second.first + data2Loader->getData().at(i2).second.second)/2);
        ui->levelBox->setEnabled(true);
        ui->levelSpinBox->setEnabled(true);
        ui->swapButton->setEnabled(true);
        if(ui->levelBox->isChecked()){
            level = new QtCharts::QLineSeries();
            if(ui->file1Box->isChecked()){
                level->append(data.at(i1).first, axisY->min());
                level->append(data.at(i1).first, axisY->max());
            }else{
                level->append(data.at(i2).first, axisY->min());
                level->append(data.at(i2).first, axisY->max());
            }
            chart->addSeries(level);
            level->attachAxis(axisY);
            level->attachAxis(axisX);
        }
        if(ui->angle1Box->isChecked()){
            phi1 = M_PI/2;
            phi2 = qAcos(intens.second/intens.first);
            ui->angle2CalcLable->setText("Unknown");
            ui->angle1CalcLable->setText(QString::number(phi2*180/M_PI));
        }else{
            phi1 = qAcos(intens.first/intens.second);
            ui->angle1CalcLable->setText("Unknown");
            ui->angle2CalcLable->setText(QString::number(phi1*180/M_PI));
            phi2 = M_PI/2;
        }
    }else{
        ui->levelBox->setEnabled(false);
        ui->levelSpinBox->setEnabled(false);
        ui->swapButton->setEnabled(false);
    }
    if(ui->calculateBox->isChecked()){
        if(!calc1){
            ui->errorLable->setText("integrate 1 before");
        }else{
            if(!calc2){
                ui->errorLable->setText("integrate 2 before");
            }else{
                if(teta1 == -1){
                    ui->errorLable->setText("unknown angle 1");
                }else{
                    if(teta2 == -1){
                        ui->errorLable->setText("unknown angle 2");
                    }else{
                        moo = -(mOrb1*qSin(teta2)*qSin(phi2) - qSin(teta1)*qSin(phi1)*mOrb2)/(qSin(teta1)*qSin(phi1)*qCos(teta2)*qCos(phi2) - qSin(teta2)*qSin(phi2)*qCos(teta1)*qCos(phi1));//fixed
                        ui->mOrbOLabel->setText(QString::number(moo));
                        mop = (-mOrb2*qCos(teta1)*qCos(phi1) + mOrb1*qCos(teta2)*qCos(phi2))/(qSin(teta1)*qSin(phi1)*qCos(teta2)*qCos(phi2) - qSin(teta2)*qSin(phi2)*qCos(teta1)*qCos(phi1));//fixed
                        ui->mOrbPLabel->setText(QString::number(mop));
                        ms = ((2)*qCos(teta1)*qCos(phi1)*msEff2 - qSin(teta1)*qSin(phi1)*msEff2 - 2*msEff1*qCos(teta2)*qCos(phi2) + msEff1*qSin(teta2)*qSin(phi2))/((2)*qCos(phi2 - teta2)*qCos(teta1)*qCos(phi1) - qCos(phi2 - teta2)*qSin(teta1)*qSin(phi1) - 2*qCos(teta2)*qCos(phi2)*qCos(phi1 - teta1) + qSin(teta2)*qSin(phi2)*qCos(phi1- teta1));//fixed
                        ui->msLabel->setText(QString::number(ms));
                        mt = -0.2857142857*(msEff2*qCos(phi1 - teta1) - msEff1*qCos(phi2 - teta2))/((2)*qCos(phi2 - teta2)*qCos(teta1)*qCos(phi1) - qCos(phi2 - teta2)*qSin(teta1)*qSin(phi1) - 2*qCos(teta2)*qCos(phi2)*qCos(phi1 - teta1) + qSin(teta2)*qSin(phi2)*qCos(phi1- teta1)); //fixed
                        ui->mTLabel->setText(QString::number(mt));
                    }
                }
            }
        }
    }
    }else{
        chart->removeAllSeries();
        diff->removeAllSeries();
    }
}

void MainWindow::exportCharts(){
    outFile = new QFile(ui->exportLine->text());
    outFile->open(QIODevice::WriteOnly);
    outStream = new QTextStream(outFile);
    bool first = ui->file1Box->isChecked();
    *outStream << "file1 = " << file1Path << "/" << ui->file1Label->text() << "\n\n";
    *outStream << "processing params :" << "\n";
    ui->file1Box->setChecked(true);
        *outStream << "lChopLength = " << ui->bSpinBox->value() << "\n";
        *outStream << "rChopLength = " << ui->tSpinBox->value() << "\n";
        *outStream << "normalisation type = ";
        if(ui->stepBox->isChecked()){
            *outStream << "r + constant step";
        }else{
            if(ui->mulBox->isChecked()){
                *outStream << "r * constant";
            }else{
                *outStream << "none";
            }
        }
        *outStream << "\n";
        *outStream << "leftLinearisationInterval = " << ui->llSpinBox->value() << "\n";
        *outStream << "rightLinearisationInterval = " << ui->rlSpinBox->value() << "\n";
        *outStream << "dividerPos = " << ui->dividerSpinBox->value() << "\n" ;
        *outStream << "linear coeff : l = " << ui->lkLabel->text() << " r = " << ui->rkLabel->text() << "\n";
        *outStream << "fillet mult = " << ui->filletSpinBox->value() << "\n";
        *outStream << "polarisation coeff = Pc = " << ui->pSpinBox->value() << "\n";
        *outStream << "Nh = " << ui->nSpinBox->value() << "\n";

    ui->file2Box->setChecked(true);
    *outStream << "file2 = " << file2Path << "/" << ui->file2Label->text() << "\n\n";
    *outStream << "processing params :" << "\n";
    *outStream << "lChopLength = " << ui->bSpinBox->value() << "\n";
    *outStream << "rChopLength = " << ui->tSpinBox->value() << "\n";
    *outStream << "normalisation type = ";
    if(ui->stepBox->isChecked()){
        *outStream << "r + constant step";
    }else{
        if(ui->mulBox->isChecked()){
            *outStream << "r * constant";
        }else{
            *outStream << "none";
        }
    }
    *outStream << "\n";
    *outStream << "leftLinearisationInterval = " << ui->llSpinBox->value() << "\n";
    *outStream << "rightLinearisationInterval = " << ui->rlSpinBox->value() << "\n";
    *outStream << "dividerPos = " << ui->dividerSpinBox->value() << "\n" ;
    *outStream << "linear coeff : l = " << ui->lkLabel->text() << " r = " << ui->rkLabel->text() << "\n";
    *outStream << "fillet mult = " << ui->filletSpinBox->value() << "\n";
    *outStream << "polarisation coeff = Pc = " << ui->pSpinBox->value() << "\n";
    *outStream << "Nh = " << ui->nSpinBox->value() << "\n";
    if(first){
        ui->file1Box->setChecked(true);
    }
    *outStream << "results:" << "\n";
    *outStream << "first:" << "\n";
    *outStream << "Al3 + Al2 = " << ui->sum1Label->text() << "\n";
    *outStream << "dAl3 = " << ui->l31Label->text() << "\n";
    *outStream << "dAl2 = " << ui->l21Label->text() << "\n";
    *outStream << "msEff = " << msEff1 << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "mOrb = " << mOrb1 << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "second:" << "\n";
    *outStream << "Al3 + Al2 = " << ui->sum2Label->text() << "\n";
    *outStream << "dAl3 = " << ui->l32Label->text() << "\n";
    *outStream << "dAl2 = " << ui->l22Label->text() << "\n";
    *outStream << "msEff = " << msEff2 << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "mOrb = " << mOrb2 << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "final:" << "\n";
    *outStream << "ms = " << ui->msLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "mT = " << ui->mTLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "mOrbOrt = " << ui->mOrbOLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
    *outStream << "mOrbPar = " << ui->mOrbPLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n\n";
    *outStream << "legend:" << "\n" ;
    *outStream << "x = wavelength (column 2 in original file)" << "\n" ;
    *outStream << "l = l bare data (column 3 in original file)" << "\n" ;
    *outStream << "r = r bare data (column 7 in original file)" << "\n" ;
    *outStream << "lN = normalised, maybe stepped down data (column 6 in original file)" << "\n" ;
    *outStream << "rN = normalised, maybe stepped down data (column 10 in original file)" << "\n" ;
    *outStream << "l Max = l maximums" << "\n" ;
    *outStream << "r Max = r maximums" << "\n" ;
    *outStream << "diff = rN - lN"  << "\n\n";
    for(int k = 0; k < 3; k++){
        QString temp = ui->exportLine->text();
        temp.chop(4);
        switch (k) {
            case 0:
                break;
            case 1:
                outFile->close();
                outFile = new QFile(temp + "_1.txt");
                outFile->open(QIODevice::WriteOnly);
                outStream = new QTextStream(outFile);
                break;
            case 2:
                outFile->close();
                outFile = new QFile(temp + "_2.txt");
                outFile->open(QIODevice::WriteOnly);
                outStream = new QTextStream(outFile);
                break;
        }
    for(int i = 1; i < 3; i++){
        *outStream << "#" << i << "file\n";
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

    if(i == 1){
        data = data1Loader->getData();
    }else{
        data = data2Loader->getData();
    }
    for(int j = 0; j < data.length(); j++){
        *outStream  << qSetFieldWidth(10) << left << data.at(j).first;
        *outStream << qSetFieldWidth(14) << left;
        if(ui->rawBox->isChecked()){
            *outStream << bareData.at(j).second.first;
            *outStream << bareData.at(j).second.second;
        }
        if(ui->normBox->isChecked()){
            *outStream << data.at(j).second.first;
            *outStream << data.at(j).second.second;
        }
        if(ui->maxBox->isChecked()){
            if(lInd.first == j || lInd.second == j){
                *outStream << data.at(j).second.first;
            }else{
                *outStream << 0;
            }
            if(rInd.first == j || rInd.second == j){
                *outStream << data.at(j).second.second;
            }else{
                *outStream << 0;
            }
        }
        if(ui->diffBox->isChecked()){
            *outStream << (data.at(j).second.first - data.at(j).second.second);
        }
        *outStream << qSetFieldWidth(1) << "\n";
    }
    }
    *outStream << qSetFieldWidth(4) << "\nEOF";
    }
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


void MainWindow::lChanged(){
    if(!ui->linearBackgroundBox->isChecked()){
        ui->lkLabel->setText("Unknown");
        ui->rkLabel->setText("Unknown");
        if(ui->file1Box->isChecked()){
            ui->sum1Label->setText("Unknown");
            ui->l31Label->setText("Unknown");
            ui->l21Label->setText("Unknown");
        }else{
            ui->sum2Label->setText("Unknown");
            ui->l32Label->setText("Unknown");
            ui->l22Label->setText("Unknown");
        }
        ui->mTLabel->setText("Unknown");
        ui->msLabel->setText("Unknown");
        ui->mOrbOLabel->setText("Unknown");
        ui->mOrbPLabel->setText("Unknown");
    }
    reCalc();
}

void MainWindow::bg(){
    if((ui->linearBackgroundBox->isChecked() || ui->steppedBackgroundBox->isChecked()) &&!(ui->normBox->isChecked()
    || ui->stepBox->isChecked() || ui->mulBox->isChecked() || ui->oneBox->isChecked())){
        ui->normBox->setChecked(true);
    }
    reCalc();
}

void MainWindow::fileSelect(){
    if(loaded1){
        if(ui->file1Box->isChecked()){
            chart->setTitle("XAS file 1");
            diff->setTitle("XMCD file 1");
        }else{
            chart->setTitle("XAS file 2");
            diff->setTitle("XMCD file 2");
        }
        open(ui->fileTree->currentIndex());
        qreal tmp = state->value("linL", 0);
        state->insert("linL", ui->llSpinBox->value());
        ui->llSpinBox->setValue(tmp);
        tmp = state->value("linR", 0);
        state->insert("linR", ui->rlSpinBox->value());
        ui->rlSpinBox->setValue(tmp);
        tmp = state->value("div", 0);
        state->insert("div", ui->dividerSpinBox->value());
        tmp = state->value("chopL", 0);
        state->insert("chopL", ui->bSpinBox->value());
        ui->bSpinBox->setValue(tmp);
        tmp = state->value("chopR", 0);
        state->insert("chopR", ui->tSpinBox->value());
        ui->tSpinBox->setValue(tmp);
        tmp = state->value("smooth", 10);
        state->insert("smooth", ui->filletSpinBox->value());
        ui->filletSpinBox->setValue(tmp);
        bool raw = state->value("raw", 0) == 1;
        if(ui->rawBox->isChecked()){
            state->insert("raw", 1);
        }else{
            state->insert("raw", 0);
        }
        bool zero = state->value("zero", 0) == 1;
        if(ui->zeroBox->isChecked()){
            state->insert("zero", 1);
        }else{
            state->insert("zero", 0);
        }
        bool norm = state->value("norm", 1) == 1; //default
        if(ui->normBox->isChecked()){
            state->insert("norm", 1);
        }else{
            state->insert("norm", 0);
        }
        bool plus = state->value("plus", 0) == 1;
        if(ui->stepBox->isChecked()){
            state->insert("plus", 1);
        }else{
            state->insert("plus", 0);
        }
        bool mul = state->value("mul", 0) == 1;
        if(ui->mulBox->isChecked()){
            state->insert("mul", 1);
        }else{
            state->insert("mul", 0);
        }
        bool one = state->value("one", 0) == 1;
        if(ui->oneBox->isChecked()){
            state->insert("one", 1);
        }else{
            state->insert("one", 0);
        }
        tmp = state->value("max", 0);
        if(ui->maxBox->isChecked()){
            state->insert("max", 1);
        }else{
            state->insert("max", 0);
        }
        ui->maxBox->setChecked(1 == tmp);
        bool lin = state->value("lin", 0) == 1;
        if(ui->linearBackgroundBox->isChecked()){
            state->insert("lin", 1);
        }else{
            state->insert("lin", 0);
        }
        bool st = state->value("st", 0) == 1;
        if(ui->steppedBackgroundBox->isChecked()){
            state->insert("st", 1);
        }else{
            state->insert("st", 0);
        }
        bool diff = state->value("diff", 0) == 1;
        if(ui->diffBox->isChecked()){
            state->insert("diff", 1);
        }else{
            state->insert("diff", 0);
        }
        bool integr = state->value("integr", 0) == 1;
        if(ui->integrateBox->isChecked()){
            state->insert("integr", 1);
        }else{
            state->insert("integr", 0);
        }
        tmp = state->value("linShow", 0);
        if(ui->linearBox->isChecked()){
            state->insert("linShow", 1);
        }else{
            state->insert("linShow", 0);
        }
        ui->linearBox->setChecked(1 == tmp);
        tmp = state->value("maxShow", 0);
        if(ui->dividerBox->isChecked()){
            state->insert("maxShow", 1);
        }else{
            state->insert("maxShow", 0);
        }
        ui->dividerBox->setChecked(1 == tmp);
        ui->rawBox->setChecked(raw);
        ui->zeroBox->setChecked(zero);
        ui->normBox->setChecked(norm);
        ui->stepBox->setChecked(plus);
        ui->mulBox->setChecked(mul);
        ui->oneBox->setChecked(one);
        ui->linearBackgroundBox->setChecked(lin);
        ui->steppedBackgroundBox->setChecked(st);
        ui->diffBox->setChecked(diff);
        ui->integrateBox->setChecked(integr);
        reCalc();
    }else{
        ui->file1Box->setChecked(true);
    }
}

void MainWindow::forget1(){
    ui->file1Label->setText("none");
    loaded1 = false;
    calc1 = false;
    mOrb1 = 0;
    msEff1 = 0;
    file1Name = "";
    file1Path = "";
    open(ui->fileTree->currentIndex());
    if(!ui->file2Box->isChecked()){
        ui->diffBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->normBox->setChecked(true);
        QtCharts::QAbstractSeries *delTmp;
        foreach(delTmp, chart->series()){
            chart->removeSeries(delTmp);
        }
        foreach(delTmp, diff->series()){
            chart->removeSeries(delTmp);
        }
    }
    ui->file1Box->setChecked(true);
    reCalc();
}

void MainWindow::forget2(){
    ui->file2Label->setText("none");
    loaded2 = false;
    calc2 = false;
    mOrb2 = 0;
    msEff2 = 0;
    file2Name = "";
    file2Path = "";
    open(ui->fileTree->currentIndex());
    if(!ui->file1Box->isChecked()){
        ui->diffBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->normBox->setChecked(true);
        QtCharts::QAbstractSeries *delTmp;
        foreach(delTmp, chart->series()){
            chart->removeSeries(delTmp);
        }
        foreach(delTmp, diff->series()){
            chart->removeSeries(delTmp);
        }
    }
    reCalc();
}

void MainWindow::reCalcBoth(){
    if(loaded2){
        ui->file2Box->setChecked(true);
        reCalc();
    }
    if(loaded1){
        ui->file1Box->setChecked(true);
        reCalc();
    }
    reCalc();
}
