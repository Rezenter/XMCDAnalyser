#include "mainwindow.h"
#include <complex>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    /*To-do list:
        * carefully delete everything in destructor
        * prevent crash
    */

    ui->setupUi(this);
    session = new QSettings(QApplication::applicationDirPath() + "/session.ini", QSettings::IniFormat);
    session->setValue("exePath", path);
    settings = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    par = new QSettings(QApplication::applicationDirPath() + "/par.ini", QSettings::IniFormat);//wtf
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
    axisY->setGridLineColor(QColor(150, 250 , 150));
    axisY->setLinePenColor(QColor(0, 150 , 0));
    axisY2 = new QtCharts::QValueAxis();
    axisY2->setGridLineColor(QColor(150, 150 , 250));
    axisY2->setLinePenColor(QColor(0, 0 , 150));
    diffY = new QtCharts::QValueAxis();
    diffY->setMinorTickCount(2);
    diffY->setTickCount(10);
    diffY->setTitleText("Intensity XMCD, arb.u.");
    axisX->setMinorTickCount(4);
    axisX->setTickCount(20);
    axisX->setTitleText("primary photons energy, eV");
    diffX->setMinorTickCount(4);
    diffX->setTickCount(20);
    diffX->setTitleText("primary photons energy, eV");
    axisY->setMinorTickCount(2);
    axisY->setTickCount(10);
    axisY->setTitleText("Intensity XAS, arb.u.");
    axisY2->setMinorTickCount(2);
    axisY2->setTickCount(11);
    axisY2->setTitleText("Intensity XMCD, arb.u.");
    chartView = new QtCharts::QChartView(chart, ui->chartWidget);
    diffView = new QtCharts::QChartView(diff, ui->diffWidget);
    diff->legend()->setAlignment(Qt::AlignLeft);
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
    QObject::connect(ui->tabWidget, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), this, &MainWindow::myResize);
    QObject::connect(ui->levelBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    QObject::connect(ui->swapButton, &QPushButton::pressed, this, &MainWindow::swap);
    QObject::connect(ui->energyShiftSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalcBoth);
    QObject::connect(ui->sessionButton, &QPushButton::pressed, this, &MainWindow::loadSession);
    QObject::connect(ui->sumBox, &QCheckBox::clicked, this, &MainWindow::reCalc);
    QObject::connect(ui->shadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->zeroShadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reCalc);
    QObject::connect(ui->treatAngleAsGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::reopen);
    buildFileTree();
    loadSettings();
    refresh();
    if(!session->value("normalExit", true).toBool()){
        loadSession();
    }
    session->setValue("normalExit", false);

    ui->sumBox->setEnabled(false); //disabled in current version
    ui->angle1Box->setEnabled(false); //disabled in current version
    ui->angle2Box->setEnabled(false); //disabled in current version
    ui->sessionButton->setEnabled(false); //disabled in current version

}

MainWindow::~MainWindow()// recheck the system
{
    //session->setValue("normalExit", true);//debug
    delete session;
    saveSettings();
    delete settings;
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
    if(settings->value("useHolders", false).toBool()){
        ui->holderButton->setChecked(true);
    }else{
        ui->angleButton->setChecked(true);
    }
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
    //ui->levelSpinBox->setEnabled(false);
    //ui->levelBox->setEnabled(false);
    myResize();
}

void MainWindow::saveSettings(){
    settings->beginGroup("gui");
    settings->setValue("windowSize", this->size());
    settings->setValue("windowPosition", this->pos());
    settings->setValue("windowTitle", this->windowTitle());
    settings->setValue("lastDrive", ui->driveBox->currentText());
    settings->setValue("useHolders", ui->holderButton->isChecked());
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
    ui->exportLine->setText(file1Path + "/" + sample + "_output");
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
        if(ui->holderButton->isChecked()){
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
        if(ui->holderButton->isChecked()){
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
    //chart->legend()->hide();
    //diff->legend()->hide();
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
    /*qDebug() << "reCalc" << count;
    count++;
    */
    if((ui->file1Box->isChecked() && loaded1) || (ui->file2Box->isChecked() && loaded2)){
        if(ui->rawBox->isChecked() || ui->zeroBox->isChecked()){
        ui->linearBox->setChecked(false);
        ui->linearBackgroundBox->setChecked(false);
        ui->steppedBackgroundBox->setChecked(false);
    }
    QVector<QPair<qreal, QPair<qreal, qreal>>> zeroData;
    if(ui->file1Box->isChecked()){
        data1Loader->setLimits(ui->bSpinBox->value(), ui->tSpinBox->value(), ui->energyShiftSpinBox->value()/2.0);
        zeroData = data1Loader->getZero();
        bareData = data1Loader->getBareData();
    }else{
        data2Loader->setLimits(ui->bSpinBox->value(), ui->tSpinBox->value(), -ui->energyShiftSpinBox->value()/2.0);
        zeroData = data2Loader->getZero();
        bareData = data2Loader->getBareData();
    }
    data.clear();
    for(int i = 0; i < bareData.length(); i++){
        data.append(QPair<qreal, QPair<qreal, qreal>>(bareData.at(i).first, QPair<qreal, qreal>((bareData.at(i).second.first - ui->shadowSpinBox->value()*1E-12)/(zeroData.at(i).second.first - ui->zeroShadowSpinBox->value()*1E-6), (bareData.at(i).second.second - ui->shadowSpinBox->value()*1E-12)/(zeroData.at(i).second.second - ui->zeroShadowSpinBox->value()*1E-12))));
    }
    ui->llSpinBox->setMinimum(data.at(2).first - data.first().first);
    if(chart->axes().contains(axisX)){
        chart->removeAxis(axisX);
    }
    if(chart->axes().contains(axisY)){
        chart->removeAxis(axisY);
    }
    if(chart->axes().contains(axisY2)){
        chart->removeAxis(axisY2);
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
        ui->dividerSpinBox->setValue(axisX->min() + (axisX->max() - axisX->min())/2.0);
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
    qreal min = 1E12;
    qreal max = -1E12;
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addAxis(axisY2, Qt::AlignRight);
    diff->addAxis(diffX, Qt::AlignBottom);
    diff->addAxis(diffY, Qt::AlignLeft);
    QPair<qreal, QPair<qreal, qreal>> pair;
    if(bareData.length() != 0 && ui->rawBox->isChecked()){
        r = new QtCharts::QLineSeries();
        l = new QtCharts::QLineSeries();
        r->setName("Raw R");
        l->setName("Raw L");
        int j = 0;
        foreach(pair, bareData){
            l->append(pair.first, pair.second.first);
            r->append(pair.first, pair.second.second);
            if(min > l->points().last().y()){
                min = l->points().last().y();
            }
            if(max < l->points().last().y()){
                max = l->points().last().y();
            }
            if(min > r->points().last().y()){
                min = r->points().last().y();
            }
            if(max < r->points().last().y()){
                max = r->points().last().y();
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
    qreal tmp1 = 1;
    qreal tmp2 = 1;
    qreal step1 = 0;
    qreal step2 = 0;
    if(ui->stepBox->isChecked()){
        step2 = data.first().second.first - data.first().second.second;
    }else if(ui->mulBox->isChecked()){
        tmp1 = data.first().second.first/data.first().second.second;
        tmp2 = data.last().second.first/data.last().second.second;
    }
    qreal a = (tmp2 - tmp1)/(data.last().first - data.first().first);
    qreal b = tmp1 - (tmp2- tmp1)*data.first().first/(data.last().first - data.first().first);
    if(ui->mulBox->isChecked() || ui->stepBox->isChecked()){
        for(int c = 0; c < data.length(); c++){
            data.replace(c, QPair<qreal, QPair<qreal, qreal>>(data.at(c).first, QPair<qreal, qreal>(data.at(c).second.first + step1, data.at(c).second.second * (a*data.at(c).first + b) + step2)));
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
        lIntervals->setName("Lin. Interval");
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
                ylSum += (pair.second.first + pair.second.second)/2.0;
                xxlSum += pow(pair.first, 2);
                xylSum += pair.first*(pair.second.first + pair.second.second)/2.0;
            }
            if(ui->rlSpinBox->value() > 0 && pair.first > data.last().first - ui->rlSpinBox->value()){
                rn++;
                xrSum += pair.first;
                yrSum += (pair.second.first + pair.second.second)/2.0;
                xxrSum += pow(pair.first, 2);
                xyrSum += pair.first*(pair.second.first + pair.second.second)/2.0;
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
            ll->setName("L. Linearization");
            a = al;
            b = bl;
        }
        if(rn > 1){
            ar = (rn*xyrSum - xrSum*yrSum)/(rn*xxrSum - pow(xrSum, 2));
            br = (yrSum - ar*xrSum)/rn;
            rl = new QtCharts::QLineSeries();
            rl->setName("R. Linearization");
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
    findMax();
    if(ui->steppedBackgroundBox->isChecked()){
        ui->linearBackgroundBox->setChecked(true);
        if(chart->series().contains(rl)){
            chart->removeSeries(rl);
        }
        qreal m1 = ((data.last().second.first - data.first().second.first)*2.0/3.0)/(2.0*qAcos(0.0));
        qreal m2 = ((data.last().second.first - data.first().second.first)/3.0)/(2.0*qAcos(0.0));
        for(int i = 0; i < data.length(); i++){
            qreal tmp = data.first().second.first + m1 * (qAcos(0.0) + qAtan(ui->filletSpinBox->value()*(data.at(i).first - data.at(lInd.first).first))) + m2 * (qAcos(0.0) + qAtan(ui->filletSpinBox->value()*(data.at(i).first - data.at(lInd.second).first)));
            data.replace(i, QPair<qreal, QPair<qreal, qreal>>(data.at(i).first, QPair<qreal, qreal>(data.at(i).second.first - tmp, data.at(i).second.second - tmp)));
        }
    }
    if(chart->series().contains(halfSum)){
        chart->removeSeries(halfSum);
    }
    /*
    if(ui->sumBox->isChecked()){
        halfSum = new QtCharts::QLineSeries();
        halfSum->setName("Half-Sum");
        qreal halfMax = 0;
        for(int i = 0; i < data.length(); i++){
            halfSum->append(data.at(i).first, (data.at(i).second.first + data.at(i).second.second)/2.0);
            if(halfMax < halfSum->points().last().y()){
                halfMax = halfSum->points().last().y();
            }
        }
        for(int i = 0; i < data.length(); i++){
            data.replace(i, QPair<qreal, QPair<qreal, qreal>>(data.at(i).first, QPair<qreal, qreal>(data.at(i).second.first/halfMax, data.at(i).second.second/halfMax)));
            halfSum->replace(i, QPointF(halfSum->at(i).x(), halfSum->at(i).y()/halfMax));
        }
        halfSum->setColor(QColor(1, 1, 1));
        chart->addSeries(halfSum);
        halfSum->attachAxis(axisY);
        halfSum->attachAxis(axisX);
    }
    */
    if(ui->file1Box->isChecked()){
        tmp1Data = data;
    }else{
        tmp2Data = data;
    }
    if(data.length() != 0 && !(ui->rawBox->isChecked() || ui->zeroBox->isChecked())){
        lNorm = new QtCharts::QLineSeries();
        lNorm->setName("L. Normalized");
        rNorm = new QtCharts::QLineSeries();
        rNorm->setName("R. Normalized");
        if(ui->file1Box->isChecked()){
            limits1 = QPair<qreal, qreal>(data.first().first, data.last().first);
        }else{
            limits2 = QPair<qreal, qreal>(data.first().first, data.last().first);
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
    if(data.length() != 0 && ui->zeroBox->isChecked()){
        lz = new QtCharts::QLineSeries();
        lz->setName("L. I-zero");
        rz = new QtCharts::QLineSeries();
        rz->setName("R. I-zero");
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
    findMax();
    axisY->setMin(min);
    axisY->setMax(max);

    if(chart->series().contains(lMax)){
        chart->removeSeries(lMax);
    }
    if(chart->series().contains(rMax)){
        chart->removeSeries(rMax);
    }
    rMax = new QtCharts::QLineSeries();
    rMax->setName("R. Max");
    lMax = new QtCharts::QLineSeries();
    lMax->setName("L. Max");
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
    divider->setName("Max. Separator");
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
    if(diff->series().contains(diffS2)){
        diff->removeSeries(diffS2);
    }
    if(diff->series().contains(l0)){
        diff->removeSeries(l0);
    }
    if(chart->series().contains(chartDiff)){
        chart->removeSeries(chartDiff);
    }
    if(chart->series().contains(XMCDZero)){
        chart->removeSeries(XMCDZero);
    }
    if(ui->diffBox->isChecked()){
        diffS = new QtCharts::QLineSeries();
        diffS2 = new QtCharts::QLineSeries();
        chartDiff = new QtCharts::QLineSeries();
        XMCDZero = new QtCharts::QLineSeries();
        XMCDZero->setName("XMCD Zero");
        l0 = new QtCharts::QLineSeries();
        l0->setName("0");
        min = 1;
        max = -1;
        qreal tmp;
        if(loaded1&&loaded2){
            foreach (pair, tmp1Data) {
                tmp = pair.second.second - pair.second.first;
                if(tmp > max){
                    max = tmp;
                }else{
                    if(tmp < min){
                        min = tmp;
                    }
                }
                diffS->append(pair.first, tmp);
                if(ui->file1Box->isChecked()){
                    chartDiff->append(pair.first, tmp);
                }
                l0->append(pair.first, 0);
            }
            diffS->setName("file 1");
            foreach (pair, tmp2Data) {
                tmp = pair.second.second - pair.second.first;
                if(tmp > max){
                    max = tmp;
                }else{
                    if(tmp < min){
                        min = tmp;
                    }
                }
                diffS2->append(pair.first, tmp);
                if(ui->file2Box->isChecked()){
                    chartDiff->append(pair.first, tmp);
                }
            }
            diffS2->setName("file 2");
        }else{
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
                chartDiff->append(pair.first, tmp);
                l0->append(pair.first, 0);
            }
            if(ui->file1Box->isChecked()){
                diffS->setName("file 1");
            }else{
                diffS->setName("file 2");
            }
        }
        chartDiff->setName("XMCD");
        XMCDZero->append(axisX->min(), 0);
        XMCDZero->append(axisX->max(), 0);
        diffY->setMin(min);
        diffY->setMax(max);
        diff->addSeries(diffS);
        diff->addSeries(l0);
        l0->attachAxis(diffY);
        l0->attachAxis(diffX);
        diffS->attachAxis(diffY);
        diffS->attachAxis(diffX);
        diff->addSeries(diffS2);
        diffS2->attachAxis(diffY);
        diffS2->attachAxis(diffX);
        chart->addSeries(chartDiff);
        axisY2->setMin(min);
        axisY2->setMax(max);
        chartDiff->attachAxis(axisY2);
        chartDiff->attachAxis(axisX);
        chart->addSeries(XMCDZero);
        XMCDZero->attachAxis(axisX);
        XMCDZero->attachAxis(axisY2);
    }
    if(ui->integrateBox->isChecked()){
        if(!ui->steppedBackgroundBox->isChecked() || !ui->linearBackgroundBox->isChecked() || !ui->diffBox->isChecked()){
            ui->integrateBox->setChecked(false);
        }else{
            qreal summ = 0;
            for(int i = 0; i < data.length() - 1; i++){
                summ += (data.at(i + 1).first - data.at(i).first)*(data.at(i + 1).second.first + data.at(i).second.first)/2.0;
                summ += (data.at(i + 1).first - data.at(i).first)*(data.at(i + 1).second.second + data.at(i).second.second)/2.0;
            }
            qreal localMin = 1E12;
            int localMinInd = 0;
            for(int i = lInd.first; i < lInd.second; i++){
                if((data.at(i).second.first + data.at(i).second.second)/2.0 < localMin){
                   localMin = (data.at(i).second.first + data.at(i).second.second)/2.0;
                   localMinInd = i;
                }
            }
            qreal dl3 = 0;
            for(int i = 0; i < localMinInd - 1; i++){
                dl3 += (diffS->at(i + 1).x() - diffS->at(i).x())*(diffS->at(i + 1).y() + diffS->at(i).y())/2.0;
            }
            qreal dl2 = 0;
            for(int i = localMinInd; i < data.length() - 1; i++){
                dl2 += (diffS->at(i + 1).x() - diffS->at(i).x())*(diffS->at(i + 1).y() + diffS->at(i).y())/2.0;
            }
            if(ui->file1Box->isChecked()){
                msEff1 = -2.0*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 - 2.0*dl2)/summ;
                mOrb1 = -(4.0/3.0)*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 + dl2)/summ;
                ui->l31Label->setText(QString::number(dl3));
                ui->sum1Label->setText(QString::number(summ));
                ui->l21Label->setText(QString::number(dl2));
                ui->msEff1Label->setText(QString::number(msEff1) + " " + units[ui->unitBox->currentIndex()]);
                ui->mOrb1Label->setText(QString::number(mOrb1) + " " + units[ui->unitBox->currentIndex()]);
                calc1 = true;
            }else{
                msEff2 = -2.0*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 - 2.0*dl2)/summ;
                mOrb2 = -(4.0/3.0)*(ui->nSpinBox->value()*muB[ui->unitBox->currentIndex()]/ui->pSpinBox->value())*(dl3 + dl2)/summ;
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
        ui->msLabel->setText("Unknown");
        ui->mTLabel->setText("Unknown");
        ui->mOrbOLabel->setText("Unknown");
        ui->mOrbPLabel->setText("Unknown");
        ui->mOrbLabel->setText("Unknown");
        if(ui->file1Box->isChecked()){
            ui->sum1Label->setText("Unknown");
            ui->l31Label->setText("Unknown");
            ui->l21Label->setText("Unknown");
            ui->msEff1Label->setText("Unknown");
            ui->mOrb1Label->setText("Unknown");
            calc1 = false;
        }else{
            ui->sum2Label->setText("Unknown");
            ui->l32Label->setText("Unknown");
            ui->l22Label->setText("Unknown");
            ui->msEff2Label->setText("Unknown");
            ui->mOrb2Label->setText("Unknown");
            calc2 = false;
        }
    }
    if(chart->series().contains(level)){
        chart->removeSeries(level);
    }
    if(diff->series().contains(dLevel)){
        diff->removeSeries(dLevel);
    }
    if(loaded1 && loaded2){
        ui->swapButton->setEnabled(true);
        ui->levelBox->setEnabled(true);
        if(ui->levelBox->isChecked()){
            int minInd1 = 0;
            qreal signalMin1 = 10000;
            for(int i = 0; i < tmp1Data.length(); i++) {
                QPair<qreal,QPair<qreal, qreal>> curr = tmp1Data.at(i);
                if((curr.second.second - curr.second.first) < signalMin1){
                    minInd1 = i;
                    signalMin1 = curr.second.second - curr.second.first;
                }
            }
            int minInd2 = 0;
            qreal signalMin2 = 10000;
            for(int i = 0; i < tmp2Data.length(); i++) {
                QPair<qreal,QPair<qreal, qreal>> curr = tmp2Data.at(i);
                if((curr.second.second - curr.second.first) < signalMin2){
                    minInd2 = i;
                    signalMin2 = curr.second.second - curr.second.first;
                }
            }
            qreal signalMin = (tmp1Data.at(minInd1).first + tmp2Data.at(minInd2).first)/2;
            minInd1 = 0;
            for(int i = 0; i < tmp1Data.length(); i++){
                if(qAbs(signalMin - tmp1Data.at(minInd1).first) > qAbs(signalMin - tmp1Data.at(i).first)){
                    minInd1 = i;
                }
            }
            signalMin1 = tmp1Data.at(minInd1).second.second - tmp1Data.at(minInd1).second.first;
            minInd2 = 0;
            for(int i = 0; i < tmp2Data.length(); i++){
                if(qAbs(signalMin - tmp2Data.at(minInd2).first) > qAbs(signalMin - tmp2Data.at(i).first)){
                    minInd2 = i;
                }
            }
            signalMin2 = tmp2Data.at(minInd2).second.second - tmp2Data.at(minInd2).second.first;
            intens = QPair<qreal, qreal>(signalMin1, signalMin2);
            dLevel = new QtCharts::QLineSeries();
            if(ui->file1Box->isChecked()){
                dLevel->append(data.at(minInd1).first, diffY->min());
                dLevel->append(data.at(minInd1).first, diffY->max());
            }else{
                dLevel->append(data.at(minInd2).first, diffY->min());
                dLevel->append(data.at(minInd2).first, diffY->max());
            }
            dLevel->setName("min");
            diff->addSeries(dLevel);
            dLevel->attachAxis(diffY);
            dLevel->attachAxis(diffX);
            level = new QtCharts::QLineSeries();
            level->setName("XMCD min");
            if(ui->file1Box->isChecked()){
                level->append(data.at(minInd1).first, axisY->min());
                level->append(data.at(minInd1).first, axisY->max());
            }else{
                level->append(data.at(minInd2).first, axisY->min());
                level->append(data.at(minInd2).first, axisY->max());
            }
            chart->addSeries(level);
            level->attachAxis(axisY);
            level->attachAxis(axisX);
        }
    }else{
        ui->levelBox->setEnabled(false);
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
                        /*
                        if(ui->angle1Box->isChecked()){
                            phi1 = teta1;
                            phi2 = abs(teta2 + qAcos(intens.second/intens.first));
                        }else{
                            phi2 = teta2;
                            phi1 = abs(teta1 + qAcos(intens.first/intens.second));
                        }
                        */
                        //assume saturation -> phi = theta
                        phi1 = teta1;
                        phi2 = teta2;

                        ui->angle1CalcLable->setText(QString::number(phi2*180/M_PI) + "°");
                        ui->angle2CalcLable->setText(QString::number(phi1*180/M_PI)  + "°");
                        moo = -(mOrb1*qSin(teta2)*qSin(phi2) - qSin(teta1)*qSin(phi1)*mOrb2)/(qSin(teta1)*qSin(phi1)*qCos(teta2)*qCos(phi2) - qSin(teta2)*qSin(phi2)*qCos(teta1)*qCos(phi1));//fixed
                        ui->mOrbOLabel->setText(QString::number(moo));
                        mop = (-mOrb2*qCos(teta1)*qCos(phi1) + mOrb1*qCos(teta2)*qCos(phi2))/(qSin(teta1)*qSin(phi1)*qCos(teta2)*qCos(phi2) - qSin(teta2)*qSin(phi2)*qCos(teta1)*qCos(phi1));//fixed
                        ui->mOrbPLabel->setText(QString::number(mop));
                        ui->mOrbLabel->setText(QString::number(qSqrt(qPow(moo, 2) + qPow(mop, 2))));
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
    saveSession();
}

void MainWindow::exportCharts(){
    QString exPath = ui->exportLine->text();
    switch (exportState) {
    case 0: //check files
        if(!loaded1 && !loaded2){
            exportState = 100;
        }
        is1 = ui->file1Box->isChecked();
        exportState++;
        exportCharts();
        break;

    case 1: //export 1 file
        qDebug() << exPath;
        if(!QDir(exPath).exists()){
            QDir().mkdir(exPath);
        }
        outFile = new QFile(exPath + "/common.txt");
        outFile->open(QIODevice::WriteOnly);
        outStream = new QTextStream(outFile);
        if(!ui->file1Box->isChecked() && loaded1){
            swap();
        }
        /*
        expSession = new QSettings(exPath + "/session.ini", QSettings::IniFormat);
        expSession->beginGroup("file1");
        */
        if(loaded1){
            /*
            expSession->setValue("path", file1Path + "/");
            expSession->setValue("name", ui->file1Label->text());
            expSession->beginGroup("buttons");
                expSession->setValue("raw", ui->rawBox->isChecked());
                expSession->setValue("iZero", ui->zeroBox->isChecked());
                expSession->setValue("norm", ui->normBox->isChecked());
                expSession->setValue("normS", ui->stepBox->isChecked());
                expSession->setValue("normM", ui->mulBox->isChecked());
                expSession->setValue("norm0", ui->oneBox->isChecked());
                expSession->setValue("maxSep", ui->dividerBox->isChecked());
                expSession->setValue("maxVal", ui->dividerSpinBox->value());
                expSession->setValue("maxShow", ui->maxBox->isChecked());
                expSession->setValue("lLinear", ui->llSpinBox->value());
                expSession->setValue("rLinear", ui->rlSpinBox->value());
                expSession->setValue("LinearShow", ui->linearBox->isChecked());
                expSession->setValue("lCrop", ui->bSpinBox->value());
                expSession->setValue("rCrop", ui->tSpinBox->value());
                expSession->setValue("linearBg", ui->linearBackgroundBox->isChecked());
                expSession->setValue("stepSmooth", ui->filletSpinBox->value());
                expSession->setValue("steppedBg", ui->steppedBackgroundBox->isChecked());
                expSession->setValue("XMCD", ui->diffBox->isChecked());
                expSession->setValue("integr", ui->rawBox->isChecked());
            expSession->endGroup();
            */
            *outStream << "file1 = " << file1Path << "/" << ui->file1Label->text() << "\n\n";
            *outStream << "theta = " << teta1*180/M_PI << "\n";
            *outStream << "lChopLength = " << ui->bSpinBox->value() << "\n";
            *outStream << "rChopLength = " << ui->tSpinBox->value() << "\n";
            *outStream << "normalisation type = ";
            if(ui->stepBox->isChecked()){
                *outStream << "r + constant step";
            }else if(ui->mulBox->isChecked()){
                *outStream << "r * constant";
            }else{
                *outStream << "none";
            }
            *outStream << "\n";
            *outStream << "leftLinearisationInterval = " << ui->llSpinBox->value() << "\n";
            *outStream << "rightLinearisationInterval = " << ui->rlSpinBox->value() << "\n";
            *outStream << "dividerPos = " << ui->dividerSpinBox->value() << "\n" ;
            *outStream << "linear coeff : l = " << ui->lkLabel->text() << " r = " << ui->rkLabel->text() << "\n";
            *outStream << "fillet mult = " << ui->filletSpinBox->value() << "\n";
            if(ui->angle1Box->isChecked()){
                *outStream << "in-plane magnetisation" << "\n";
            }else{
                *outStream << "magnetisation angle = " << ui->angle1CalcLable->text() << "\n";
            }
            *outStream << "Al3+Al2 = " << ui->sum1Label->text() << "\n";
            *outStream << "dAl3 = " << ui->l31Label->text() << "\n";
            *outStream << "dAl2 = " << ui->l21Label->text() << "\n";
            *outStream << "msEff = " << ui->msEff1Label->text() << "\n";
            *outStream << "mOrb = " << ui->mOrb1Label->text() << "\n";
        }
        //expSession->endGroup();
        exportState++;
        if(loaded2){
            swap();
            qDebug() << "swapTo2 emitted";
        }else{
            exportState++; //skip 2 file export
        }
        exportCharts();
        break;
    case 2: //export 2 file

        if(loaded2){
            /*
            expSession->beginGroup("file2");
            expSession->setValue("path", file2Path + "/");
            expSession->setValue("name", ui->file2Label->text());
            expSession->beginGroup("buttons");
                expSession->setValue("raw", ui->rawBox->isChecked());
                expSession->setValue("iZero", ui->zeroBox->isChecked());
                expSession->setValue("norm", ui->normBox->isChecked());
                expSession->setValue("norm+", ui->stepBox->isChecked());
                expSession->setValue("norm*", ui->mulBox->isChecked());
                expSession->setValue("norm0", ui->oneBox->isChecked());
                expSession->setValue("maxSep", ui->dividerBox->isChecked());
                expSession->setValue("maxVal", ui->dividerSpinBox->value());
                expSession->setValue("maxShow", ui->maxBox->isChecked());
                expSession->setValue("lLinear", ui->llSpinBox->value());
                expSession->setValue("rLinear", ui->rlSpinBox->value());
                expSession->setValue("LinearShow", ui->linearBox->isChecked());
                expSession->setValue("lCrop", ui->bSpinBox->value());
                expSession->setValue("rCrop", ui->tSpinBox->value());
                expSession->setValue("linearBg", ui->linearBackgroundBox->isChecked());
                expSession->setValue("stepSmooth", ui->filletSpinBox->value());
                expSession->setValue("steppedBg", ui->steppedBackgroundBox->isChecked());
                expSession->setValue("XMCD", ui->diffBox->isChecked());
                expSession->setValue("integr", ui->rawBox->isChecked());
                expSession->setValue("calibr", ui->levelBox->isChecked());
                expSession->setValue("calibrLevel", ui->levelSpinBox->value());
                expSession->setValue("calc", ui->calculateBox->isChecked());
            expSession->endGroup();
            */
            *outStream << "\nfile2 = " << file2Path << "/" << ui->file2Label->text() << "\n\n";
            *outStream << "theta = " << teta2*180/M_PI << "\n";
            *outStream << "lChopLength = " << ui->bSpinBox->value() << "\n";
            *outStream << "rChopLength = " << ui->tSpinBox->value() << "\n";
            *outStream << "normalisation type = ";
            if(ui->stepBox->isChecked()){
                *outStream << "r + constant step";
            }else if(ui->mulBox->isChecked()){
                *outStream << "r * constant";
            }else{
                *outStream << "none";
            }
            *outStream << "\n";
            *outStream << "leftLinearisationInterval = " << ui->llSpinBox->value() << "\n";
            *outStream << "rightLinearisationInterval = " << ui->rlSpinBox->value() << "\n";
            *outStream << "dividerPos = " << ui->dividerSpinBox->value() << "\n" ;
            *outStream << "linear coeff : l = " << ui->lkLabel->text() << " r = " << ui->rkLabel->text() << "\n";
            *outStream << "fillet mult = " << ui->filletSpinBox->value() << "\n";
            if(ui->angle2Box->isChecked()){
                *outStream << "in-plane magnetisation" << "\n";
            }else{
                *outStream << "magnetisation angle = " << ui->angle2CalcLable->text() << "\n";
            }
            *outStream << "Al3+Al2 = " << ui->sum2Label->text() << "\n";
            *outStream << "dAl3 = " << ui->l32Label->text() << "\n";
            *outStream << "dAl2 = " << ui->l22Label->text() << "\n";
            *outStream << "msEff = " << ui->msEff2Label->text() << "\n";
            *outStream << "mOrb = " << ui->mOrb2Label->text() << "\n";
        }
        //expSession->endGroup();
        exportState++;
        exportCharts();
        break;
    case 3: //export common data and charts
        *outStream << "\ncommon" << "\n";
        *outStream << "units " << ui->unitBox->currentText() << "\n";
        *outStream << "Nh = " << ui->nSpinBox->value() << "\n";
        *outStream << "polarisation coeff = Pc = " << ui->pSpinBox->value() << "\n";
        *outStream << "ms = " << ui->msLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
        *outStream << "mT = " << ui->mTLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
        *outStream << "mOrbOrt = " << ui->mOrbOLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
        *outStream << "mOrbPar = " << ui->mOrbPLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n";
        *outStream << "mOrb = " << ui->mOrbLabel->text() << " " << units[ui->unitBox->currentIndex()] << "\n\n";
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
            if(k == 1 && !loaded1){
                k++;
            }
            if(k == 2 && !loaded2){
                break;
            }
            switch (k) {
                case 1:
                    outFile->close();
                    outFile = new QFile(exPath + "/file_1.txt");
                    qDebug() << exPath + "/file_1.txt";
                    outFile->open(QIODevice::WriteOnly);
                    outStream = new QTextStream(outFile);
                    break;
                case 2:
                    outFile->close();
                    outFile = new QFile(exPath + "/file_2.txt");
                    outFile->open(QIODevice::WriteOnly);
                    outStream = new QTextStream(outFile);
                    break;
            }
        for(int i = 1; i < 3; i++){
            if(i == 1 && !loaded1){
                i++;
            }
            if(i == 2 && !loaded2){
                break;
            }
            *outStream << "#" << i << "file\n";
            *outStream << "#";
            *outStream << qSetFieldWidth(9) << center << "x";
            *outStream << qSetFieldWidth(14) << left;
        if(ui->rawBox->isChecked()){
            *outStream << "l";
            *outStream << "r";
        }
        if(ui->normBox->isChecked() || ui->stepBox->isChecked() || ui->mulBox->isChecked()){
            *outStream << "lN";
            *outStream << "rN";
        }
        if(ui->maxBox->isChecked()){
            *outStream << "l Max";
            *outStream << "r Max";
        }
        if(ui->steppedBackgroundBox->isChecked()){
            *outStream << "st. back.";
        }
        if(ui->diffBox->isChecked()){
            *outStream << "diff";
        }
        *outStream << qSetFieldWidth(1) << "\n";

        if(i == 1){
            data = tmp1Data;
        }else{
            data = tmp2Data;
        }
        qreal m1 = ((data.last().second.first - data.first().second.first)*2/3)/(2*qAcos(0));
        qreal m2 = ((data.last().second.first - data.first().second.first)/3)/(2*qAcos(0));
        for(int j = 0; j < data.length(); j++){
            *outStream  << qSetFieldWidth(10) << left << data.at(j).first;
            *outStream << qSetFieldWidth(14) << left;
            if(ui->rawBox->isChecked()){
                *outStream << bareData.at(j).second.first;
                *outStream << bareData.at(j).second.second;
            }
            if(ui->normBox->isChecked() || ui->stepBox->isChecked() || ui->mulBox->isChecked()){
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
            if(ui->steppedBackgroundBox->isChecked()){
                *outStream << data.first().second.first + m1 * (qAcos(0) + qAtan(ui->filletSpinBox->value()*(data.at(j).first - data.at(lInd.first).first))) + m2 * (qAcos(0) + qAtan(ui->filletSpinBox->value()*(data.at(j).first - data.at(lInd.second).first)));
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
        exportState++;
        exportCharts();
        break;
    default:
        if(is1 != ui->file1Box->isChecked()){
            swap();
        }
        exportState = 0; //exit
        break;
    }
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
        ui->mOrbLabel->setText("Unknown");
    }
    reCalc();
}

void MainWindow::bg(){
    if((ui->linearBackgroundBox->isChecked() || ui->steppedBackgroundBox->isChecked()) &&!(ui->normBox->isChecked()
    || ui->stepBox->isChecked() || ui->mulBox->isChecked())){
        ui->normBox->setChecked(true);
    }
    reCalc();
}

void MainWindow::fileSelect(){
    if(loaded1){
        if(ui->file1Box->isChecked()){
            chart->setTitle("XAS file 1");
            diff->setTitle("XMCD file 1");
            //ui->file2Box->setChecked(true); ?
        }else{
            chart->setTitle("XAS file 2");
            diff->setTitle("XMCD file 2");
            //ui->file1Box->setChecked(true); should i?
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
        ui->dividerSpinBox->setValue(tmp);
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
        qreal zeroShadow = state->value("zeroShadow" , 0);
        state->insert("zeroShadow", ui->zeroShadowSpinBox->value());
        qreal shadow = state->value("shadow" , 0);
        state->insert("shadow", ui->shadowSpinBox->value());
        /*bool norm = state->value("norm", 1) == 1; //default
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
        */
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
        /*bool diff = state->value("diff", 0) == 1;
        if(ui->diffBox->isChecked()){
            state->insert("diff", 1);
        }else{
            state->insert("diff", 0);
        }
        */
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
        ui->zeroShadowSpinBox->setValue(zeroShadow);
        ui->shadowSpinBox->setValue(shadow);
        /*ui->normBox->setChecked(norm);
        ui->stepBox->setChecked(plus);
        ui->mulBox->setChecked(mul);
        ui->oneBox->setChecked(one);
        */
        ui->linearBackgroundBox->setChecked(lin);
        ui->steppedBackgroundBox->setChecked(st);
        //ui->diffBox->setChecked(diff);
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

void MainWindow::loadSession(){
    /*
    //fix
    qDebug() << "loading";
    bool firstFileSelected = false;
    session->beginGroup("firstTab");
        int tmp = ui->driveBox->findText(settings->value("drive", "").toString());
        if(tmp != -1){
            ui->driveBox->setCurrentIndex(tmp);
        }
        session->beginGroup("fileTree");
            ui->fileTree->setCurrentIndex(model->index(session->value("path", "").toString()));
            session->beginGroup("filters");
                ui->sampleBox->setChecked(session->value("sample", true).toBool());
                ui->energyBox->setChecked(session->value("energy", true).toBool());
                ui->geomBox->setChecked(session->value("geometry", true).toBool());
            session->endGroup();
        session->endGroup();
        firstFileSelected = session->value("firstSelected", true).toBool();
    session->endGroup();
    session->beginGroup("common");
        if(session->value("loaded", false).toBool()){
            sample = session->value("sample", "unknown").toString();
            geom = session->value("geom", "unknown").toString();
            energy = session->value("energy", "unknown").toString();
            ui->calculateBox->setChecked(session->value("calcBox", false).toBool());
            ui->levelBox->setChecked(session->value("levelBox", false).toBool());
            ui->levelSpinBox->setValue(session->value("level", 0).toDouble());
            if(session->value("firstInPlaneM", true).toBool()){
                ui->angle1Box->setChecked(true);
            }else{
                ui->angle2Box->setChecked(true);
            }
            ui->pSpinBox->setValue(session->value("polarisation", 0.83).toDouble());
            ui->nSpinBox->setValue(session->value("Nh", 2.49).toDouble());
            ui->unitBox->setCurrentText(session->value("unit", "µβ").toString());
            ui->tabWidget->setCurrentIndex(session->value("currentTab", 0).toInt());
        }
    session->endGroup();
    session->beginGroup("file1");
        if(session->value("loaded", false).toBool()){
            //loaded1 = true;
            file1Path = settings->value("filePath", "").toString();
            file1Name = settings->value("filename", "").toString();
            if(firstFileSelected){
                //current
            }else{
                //prev
            }
            //load1
            //ui->llSpinBox
        }else{
            loaded1 = false;
        }
        //add
    session->endGroup();
    session->beginGroup("file2");
        if(session->value("loaded", false).toBool()){
            //loaded2 = true;
            file2Path = settings->value("filePath", "").toString();
            file2Name = settings->value("filename", "").toString();
            if(!firstFileSelected){
                //current
            }else{
                //prev
            }
            //load2
            //ui->llSpinBox
        }else{
            loaded1 = false;
        }
        //add
    session->endGroup();
    //firstFileSelected.
    data1Loader = new FileLoader(file1Path + "/" + file1Name);
    */
}

void MainWindow::saveSession(){
    /*
    session->beginGroup("firstTab");
        session->setValue("drive", ui->driveBox->currentText());
        session->setValue("firstSelected", ui->file1Box->isChecked());
        session->beginGroup("fileTree");
        session->remove("");
            session->setValue("path", model->filePath(ui->fileTree->currentIndex()));
            session->beginGroup("filters");
                session->setValue("sample", ui->sampleBox->isChecked());
                session->setValue("energy", ui->energyBox->isChecked());
                session->setValue("geometry", ui->geomBox->isChecked());
            session->endGroup();
        session->endGroup();
    session->endGroup();
    session->beginGroup("common");
        if(loaded1 || loaded2){
            session->setValue("loaded", true);
            session->setValue("sample", sample);
            session->setValue("geom", geom);
            session->setValue("energy", energy);
            session->setValue("calcBox", ui->calculateBox->isChecked());
            session->setValue("levelBox", ui->levelBox->isChecked());
            session->setValue("level", ui->levelSpinBox->value());
            session->setValue("firstInPlaneM", ui->angle1Box->isChecked());
            session->setValue("polarisation", ui->pSpinBox->value());
            session->setValue("Nh", ui->nSpinBox->value());
            session->setValue("unit", ui->unitBox->currentText());
            session->setValue("currentTab", ui->tabWidget->currentIndex());
        }else{
            session->remove("");
            session->setValue("loaded", false);
        }
    session->endGroup();
    session->beginGroup("file1");
        if(loaded1){
            if(!ui->file1Box->isChecked()){
                swap();
            }
            session->setValue("loaded", loaded1);
            session->setValue("filePath", file1Path);
            session->setValue("filename", file1Name);
            session->setValue("linL", ui->llSpinBox->value());
            session->setValue("linR", ui->rlSpinBox->value());
            session->setValue("div", ui->dividerSpinBox->value());
            session->setValue("chopL", ui->bSpinBox->value());
            session->setValue("chopR", ui->tSpinBox->value());
            session->setValue("smooth", ui->filletSpinBox->value());
            session->setValue("rawBox", ui->rawBox->isChecked());
            session->setValue("zeroBox", ui->zeroBox->isChecked());
            session->setValue("normBox", ui->normBox->isChecked());
            session->setValue("plusBox", ui->stepBox->isChecked());
            session->setValue("mulBox", ui->mulBox->isChecked());
            session->setValue("oneBox", ui->oneBox->isChecked());
            session->setValue("maxBox", ui->maxBox->isChecked());
            session->setValue("linBBox", ui->linearBackgroundBox->isChecked());
            session->setValue("stepBBox", ui->steppedBackgroundBox->isChecked());
            session->setValue("diffBox", ui->diffBox->isChecked());
            session->setValue("intBox", ui->integrateBox->isChecked());
            session->setValue("linSBox", ui->linearBox->isChecked());
            session->setValue("maxSBox", ui->dividerBox->isChecked());
        }else{
            session->remove("");
            session->setValue("loaded", false);
        }
    session->endGroup();
    session->beginGroup("file2");
        session->setValue("loaded", loaded2);
        if(loaded2){
            session->setValue("filePath", file2Path);
            session->setValue("filename", file2Name);
            session->setValue("linL", ui->llSpinBox->value());
            session->setValue("linR", ui->rlSpinBox->value());
            session->setValue("div", ui->dividerSpinBox->value());
            session->setValue("chopL", ui->bSpinBox->value());
            session->setValue("chopR", ui->tSpinBox->value());
            session->setValue("smooth", ui->filletSpinBox->value());
            session->setValue("rawBox", ui->rawBox->isChecked());
            session->setValue("zeroBox", ui->zeroBox->isChecked());
            session->setValue("normBox", ui->normBox->isChecked());
            session->setValue("plusBox", ui->stepBox->isChecked());
            session->setValue("mulBox", ui->mulBox->isChecked());
            session->setValue("oneBox", ui->oneBox->isChecked());
            session->setValue("maxBox", ui->maxBox->isChecked());
            session->setValue("linBBox", ui->linearBackgroundBox->isChecked());
            session->setValue("stepBBox", ui->steppedBackgroundBox->isChecked());
            session->setValue("diffBox", ui->diffBox->isChecked());
            session->setValue("intBox", ui->integrateBox->isChecked());
            session->setValue("linSBox", ui->linearBox->isChecked());
            session->setValue("maxSBox", ui->dividerBox->isChecked());
        }else{
            session->remove("");
            session->setValue("loaded", false);
        }
    session->endGroup();
    */
}
