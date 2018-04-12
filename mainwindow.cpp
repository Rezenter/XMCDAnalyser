#include "mainwindow.h"
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
     *
     * !!!!!!!!!swap broken
     * separate energy shift for each file
     * all rows merged in summary
        * prevent crash
        * save/load session
        * add sample filter
        * update charts only when they have changes
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
    summLabels[0] = ui->sum1Label;
    summLabels[1] = ui->sum2Label;
    dl2Labels[0] = ui->l21Label;
    dl2Labels[1] = ui->l22Label;
    dl3Labels[0] = ui->l31Label;
    dl3Labels[1] = ui->l32Label;
    mSELabels[0] = ui->msEff1Label;
    mSELabels[1] = ui->msEff2Label;
    mOLabels[0] = ui->mOrb1Label;
    mOLabels[1] = ui->mOrb2Label;
    phiLabels[0] = ui->phi1Label;
    phiLabels[1] = ui->phi2Label;
    thetaLabels[0] = ui->theta1Label;
    thetaLabels[1] = ui->theta2Label;
    relationLabels[0] = ui->relationLabel1;
    relationLabels[1] = ui->relationLabel2;
    ui->summaryButtonGroup->setId(ui->summaryRawButton, 0);
    ui->summaryButtonGroup->setId(ui->summaryZeroButton, 1);
    ui->summaryButtonGroup->setId(ui->summaryNormButton, 2);
    ui->summaryButtonGroup->setId(ui->summaryDiffButton, 3);
    refresh = new QFileSystemWatcher(this);
    ui->summaryTable->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList summaryLabels;
    summaryLabels.append("Use");
    summaryLabels.append("Name");
    summaryLabels.append("mS/mO");
    summaryLabels.append("mS");
    summaryLabels.append("mT");
    summaryLabels.append("mOrb∥");
    summaryLabels.append("mOrb⊥");
    summaryLabels.append("mOrb");
    summaryLabels.append("E. shift");
    summaryLabels.append("I. shift");
    ui->summaryTable->setColumnCount(summaryLabels.size());
    ui->summaryTable->setHorizontalHeaderLabels(summaryLabels);
    session = new QSettings(QApplication::applicationDirPath() + "/session.ini", QSettings::IniFormat);
    ui->tabWidget->setCurrentIndex(0);
    chart.setTitle("XAS");
    chart.setAxisX(&axisX);
    chart.addAxis(&axisY, Qt::AlignLeft);
    chart.addAxis(&axisY2, Qt::AlignRight);
    axisY.setGridLineColor(QColor(150, 250 , 150));
    axisY.setLinePenColor(QColor(0, 150 , 0));
    axisY.setLabelFormat("%2.2e");
    axisY2.setGridLineColor(QColor(150, 150 , 250));
    axisY2.setLinePenColor(QColor(0, 0 , 150));
    axisY2.setLabelFormat("%2.2e");
    axisX.setMinorGridLineVisible(true);
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
    ui->chartWidget->setLayout(new QGridLayout(ui->chartWidget));
    ui->chartWidget->layout()->addWidget(&chartView);
    chartView.setParent(ui->chartWidget);
    chartView.setChart(&chart);
    chartView.show();
    for(int file = 0; file < 2; file++){
        norm[file].setName("Norm. " + QString::number(file));
        raw[file].setName("Raw " + QString::number(file));
        zero[file].setName("Zero " + QString::number(file));
        line[file].setName("Linear " + QString::number(file));
        chart.addSeries(&line[file]);
        line[file].attachAxis(&axisX);
        line[file].attachAxis(&axisY);
        line[file].setVisible(false);
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
    }
    steps.setName("st. bg.");
    chart.addSeries(&steps);
    steps.attachAxis(&axisX);
    steps.attachAxis(&axisY);
    steps.setVisible(false);
    xmcd.setName("XMCD");
    chart.addSeries(&xmcd);
    xmcd.attachAxis(&axisX);
    xmcd.attachAxis(&axisY2);
    xmcd.setVisible(false);
    xmcdZero.setName("XMCD Zero");
    chart.addSeries(&xmcdZero);
    xmcdZero.attachAxis(&axisX);
    xmcdZero.attachAxis(&axisY2);
    xmcdZero.setVisible(false);
    xmcdZero.append(QPointF(axisX.min(), 0));
    xmcdZero.append(QPointF(axisX.max(), 0));
    dot.setName("Separator");
    dot.setMarkerSize(5);
    dot.append(0.0, 0.0);
    chart.addSeries(&dot);
    dot.attachAxis(&axisX);
    dot.attachAxis(&axisY2);
    dot.setVisible(false);
    axisY2.setVisible(false);
    summaryChart.setAxisX(&summaryAxisX);
    summaryChart.addAxis(&summaryAxisY, Qt::AlignLeft);
    summaryAxisY.setLabelFormat("%2.2e");
    summaryAxisX.setMinorGridLineVisible(true);
    summaryAxisX.setMinorTickCount(4);
    summaryAxisX.setTickCount(10);
    summaryAxisX.setTitleText("primary photons energy, eV");
    summaryAxisY.setMinorTickCount(2);
    summaryAxisY.setTickCount(10);
    summaryAxisY.setTitleText("Intensity, arb.u.");
    ui->summaryChartWidget->setLayout(new QGridLayout(ui->summaryChartWidget));
    ui->summaryChartWidget->layout()->addWidget(&summaryChartView);
    summaryChartView.setRenderHint(QPainter::Antialiasing);
    summaryChartView.setChart(&summaryChart);
    summaryChartView.show();
    ui->fileTable->setModel(table);
    ui->fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileTable->setSelectionMode(QAbstractItemView::SingleSelection);
    calcThread = new QThread();
    wrapper = new CalcWrapper();
    wrapper->moveToThread(calcThread);

    QObject::connect(wrapper, SIGNAL(dead()), calcThread, SLOT(quit()), Qt::QueuedConnection);
    QObject::connect(calcThread, SIGNAL(finished()), wrapper, SLOT(deleteLater()), Qt::QueuedConnection);
    QObject::connect(wrapper, SIGNAL(dead()), calcThread, SLOT(deleteLater()), Qt::QueuedConnection);

    //this to wrapper
    qRegisterMetaType<QVector<QPair<qreal,QPointF>>>("QVector<QPair<qreal,QPointF>>");
    qRegisterMetaType<QVector<QPointF>>("QVector<QPointF>");
    QObject::connect(this, &MainWindow::appendCalc, wrapper, &CalcWrapper::appendCalc, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::removeCalc, wrapper, &CalcWrapper::removeCalc, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setEnergyShift, wrapper, &CalcWrapper::setEnergyShift, Qt::QueuedConnection);//unused, depricated
    QObject::connect(this, &MainWindow::setLimits, wrapper, &CalcWrapper::setLimits, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setLoader, wrapper, &CalcWrapper::setLoader, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setSmooth, wrapper, &CalcWrapper::setSmooth, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setShadowCurrent, wrapper, &CalcWrapper::setShadowCurrent, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setNormalizationCoeff, wrapper, &CalcWrapper::setNormalizationCoeff, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setLinearIntervals, wrapper, &CalcWrapper::setLinearIntervals, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setStepped, wrapper, &CalcWrapper::setStepped, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setLin, wrapper, &CalcWrapper::setLin, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setIntegrate, wrapper, &CalcWrapper::setIntegrate, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setIntegrationConstants, wrapper, &CalcWrapper::setIntegrationConstants, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::update, wrapper, &CalcWrapper::update, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setCalculate, wrapper, &CalcWrapper::setCalculate, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setPositiveIntegrals, wrapper, &CalcWrapper::setPositiveIntegrals, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setRelativeCurv, wrapper, &CalcWrapper::setRelativeCurv, Qt::QueuedConnection);

    //wrapper to this
    QObject::connect(wrapper, &CalcWrapper::log, this, [=](QString out){
        emit log("wrapper:: " + out);
    });
    QObject::connect(wrapper, &CalcWrapper::processedData, this, [=](const QVector<QPair<qreal, QPointF>>* points,
                     const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- processedData:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        norm[0].blockSignals(true);
        norm[0].clear();
        norm[1].blockSignals(true);
        norm[1].clear();
        for(int i = 0; i < points->size(); ++i){
            norm[0].append(points->at(i).first, points->at(i).second.x());
            norm[1].append(points->at(i).first, points->at(i).second.y());
        }
        dataPointers[file].replace(id, points);
        norm[0].blockSignals(false);
        norm[1].blockSignals(false);
        norm[0].pointsReplaced();
        norm[1].pointsReplaced();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::rawData, this, [=](const QVector<QPair<qreal,QPointF>>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- rawData:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        raw[0].blockSignals(true);
        raw[0].clear();
        raw[1].blockSignals(true);
        raw[1].clear();
        for(int i = 0; i < points->size(); ++i){
            raw[0].append(points->at(i).first, points->at(i).second.x());
            raw[1].append(points->at(i).first, points->at(i).second.y());
        }
        rawPointers[file].replace(id, points);
        raw[0].blockSignals(false);
        raw[1].blockSignals(false);
        ui->smoothSpinBox->setMaximum(raw[this->file].pointsVector().size());
        ui->llSpinBox->setMaximum(raw[this->file].pointsVector().size());
        ui->rlSpinBox->setMaximum(raw[this->file].pointsVector().size());
        ui->integrationLimitSpinBox->setMaximum(raw[this->file].pointsVector().size() - 2);
        raw[0].pointsReplaced();
        raw[1].pointsReplaced();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::iZero, this, [=](const QVector<QPair<qreal,QPointF>>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- iZeroData:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        zero[0].blockSignals(true);
        zero[0].clear();
        zero[1].blockSignals(true);
        zero[1].clear();
        for(int i = 0; i < points->size(); ++i){
            zero[0].append(points->at(i).first, points->at(i).second.x());
            zero[1].append(points->at(i).first, points->at(i).second.y());
        }
        zeroPointers[file].replace(id, points);
        zero[0].blockSignals(false);
        zero[1].blockSignals(false);
        zero[0].pointsReplaced();
        zero[1].pointsReplaced();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::stepData, this, [=](const QVector<QPointF>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- stepData:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        steps.blockSignals(true);
        steps.clear();
        for(int i = 0; i < points->size(); ++i){
            steps.append(points->at(i).x(), points->at(i).y());
        }
        stepPointers[file].replace(id, points);
        steps.blockSignals(false);
        steps.pointsReplaced();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::XMCD, this, [=](const QVector<QPointF>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- XMCD:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        xmcd.blockSignals(true);
        xmcd.clear();
        qreal min = std::numeric_limits<double>::max();
        qreal max = std::numeric_limits<double>::min();
        for(int i = 0; i < points->size(); ++i){
            xmcd.append(points->at(i).x(), points->at(i).y());
            if(min > points->at(i).y()){
                min = points->at(i).y();
            }else if(max < points->at(i).y()){
                max = points->at(i).y();
            }
        }
        xmcdPointers[file].replace(id, points);
        xmcd.blockSignals(false);
        xmcd.pointsReplaced();
        xmcdZero.replace(0, QPointF(xmcd.at(0).x(), 0));
        xmcdZero.replace(1, QPointF(xmcd.at(xmcd.points().size() - 1).x(), 0));
        dot.replace(0, xmcd.at(ui->integrationLimitSpinBox->value()));
        axisY2.setRange(min, max);
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::linCoeffs, this, [=](const QPointF* left, const QPointF* right,
                     const QPointF* x, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- linCoeffs:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        Q_UNUSED(x);
        Q_UNUSED(id);
        Q_UNUSED(file);
        line[0].blockSignals(true);
        line[0].clear();
        line[1].blockSignals(true);
        line[1].clear();
        line[0].append(norm[0].at(0).x(), norm[0].at(0).x() * left->x() + left->y());
        line[0].append(norm[0].at(norm[0].points().size() - 1).x(), norm[0].at(norm[0].points().size() - 1).x() * left->x() + left->y());
        line[1].append(norm[0].at(0).x(), norm[0].at(0).x() * right->x() + right->y());
        line[1].append(norm[0].at(norm[0].points().size() - 1).x(),
                norm[0].at(norm[0].points().size() - 1).x() * right->x() + right->y());
        line[0].blockSignals(false);
        line[1].blockSignals(false);
        ui->lkLabel->setText(QString::number(left->x()));
        ui->rkLabel->setText(QString::number(right->x()));
        line[0].pointsReplaced();
        line[1].pointsReplaced();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::integrals, this, [=](const qreal* summ, const qreal* dl2, const qreal* dl3,
                     const qreal* mSE, const qreal* mO, const qreal* relation, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- integrals:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        Q_UNUSED(id);
        summLabels[file]->setText(QString::number(*summ));
        dl2Labels[file]->setText(QString::number(*dl2));
        dl3Labels[file]->setText(QString::number(*dl3));
        mSELabels[file]->setText(QString::number(*mSE));
        mOLabels[file]->setText(QString::number(*mO));
        relationLabels[file]->setText(QString::number(*relation));
    });
    QObject::connect(wrapper, &CalcWrapper::moments, this, [=](const qreal* mOP, const qreal* mOO,
                     const qreal* ms, const qreal* mt, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- moments:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        ui->mOrbPLabel->setText(QString::number(*mOP));
        ui->mOrbOLabel->setText(QString::number(*mOO));
        ui->msLabel->setText(QString::number(*ms));
        ui->mTLabel->setText(QString::number(*mt));
        ui->mOrbLabel->setText(QString::number(qSqrt(qPow(*mOP, 2) + qPow(*mOO, 2))));
        ui->summaryTable->item(id + file, 2)->setText(ui->msLabel->text());
        ui->summaryTable->item(id + file, 3)->setText(ui->mTLabel->text());
        ui->summaryTable->item(id + file, 4)->setText(ui->mOrbPLabel->text());
        ui->summaryTable->item(id + file, 5)->setText(ui->mOrbOLabel->text());
        ui->summaryTable->item(id + file, 6)->setText(ui->mOrbLabel->text());
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::completed, this, [=](const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- completed:: id == "  + QString::number(id) +
            ". file == " + QString::number(file)+ ".");
        rescale();
        updateSummary();
        if(pairs.size() != 0){
            pairs.at(id)->state[file].insert("isRaw", ui->rawBox->isChecked());
            pairs.at(id)->state[file].insert("isZero", ui->zeroBox->isChecked());
            pairs.at(id)->state[file].insert("isNorm", ui->normBox->isChecked());
            pairs.at(id)->state[file].insert("shadow", ui->shadowSpinBox->value());
            pairs.at(id)->state[file].insert("zeroShadow", ui->zeroShadowSpinBox->value());
            pairs.at(id)->state[file].insert("isMult", ui->mulBox->isChecked());
            pairs.at(id)->state[file].insert("mulCoeff", ui->mulCoeffSpinBox->value());
            pairs.at(id)->state[file].insert("relativeCurv", ui->particularCurvatureSpinBox->value());
            pairs.at(id)->state[file].insert("smooth", ui->smoothSpinBox->value());
            pairs.at(id)->state[file].insert("cropL", ui->bSpinBox->value());
            pairs.at(id)->state[file].insert("cropR", ui->tSpinBox->value());
            pairs.at(id)->state[file].insert("linearShow", ui->linearBox->isChecked());
            pairs.at(id)->state[file].insert("linearL", ui->llSpinBox->value());
            pairs.at(id)->state[file].insert("linearR", ui->rlSpinBox->value());
            pairs.at(id)->state[file].insert("steppedMul", ui->filletSpinBox->value());
            pairs.at(id)->state[file].insert("linear", ui->linearBackgroundBox->isChecked());
            pairs.at(id)->state[file].insert("stepped", ui->steppedBackgroundBox->isChecked());
            pairs.at(id)->state[file].insert("xmcd", ui->diffBox->isChecked());
            pairs.at(id)->state[file].insert("integrationLimit", ui->integrationLimitSpinBox->value());
            pairs.at(id)->state[file].insert("integrate", ui->integrateBox->isChecked());
            pairs.at(id)->state[file].insert("positiveIntegrals", ui->positiveCheckBox->isChecked());
            pairs.at(id)->state[file].insert("calc", ui->calculateBox->isChecked());
            pairs.at(id)->state[file].insert("phiOff", offsets[file]->value());
        }
    }, Qt::QueuedConnection);


    //ui to this
    QObject::connect(ui->fileTable, &QTableView::doubleClicked, this, [=](QModelIndex index){
        int file = 1;
        if(fileCheckBox[0]->isChecked()){
            file = 0;
            sample = table->data(table->index(index.row(), 1), Qt::DisplayRole).toString();
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
        theta[file] = table->data(index = table->index(index.row(), 2), Qt::DisplayRole).toDouble();
    });
    QObject::connect(ui->file1Button, &QRadioButton::toggled, this, [=]{
        open(dataDir);
    });
    QObject::connect(ui->tSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        setLimits(ui->bSpinBox->value(), val, file, id);
    });
    QObject::connect(ui->bSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        setLimits(val, ui->tSpinBox->value(), file, id);
    });
    QObject::connect(ui->rawBox, &QRadioButton::toggled, this, [=](bool state){
        raw[0].setVisible(state);
        raw[1].setVisible(state);
        if(state){
            while(!buttons.isEmpty()){
                buttons.takeLast()->setChecked(false);
            }
            buttons.append(ui->rawBox);
            rescale();
        }
    });
    QObject::connect(ui->zeroBox, &QRadioButton::toggled, this, [=](bool state){
        zero[0].setVisible(state);
        zero[1].setVisible(state);
        if(state){
            while(!buttons.isEmpty()){
                buttons.takeLast()->setChecked(false);
            }
            buttons.append(ui->zeroBox);
            rescale();
        }
    });
    QObject::connect(ui->normBox, &QRadioButton::toggled, this, [=](bool state){
        norm[0].setVisible(state);
        norm[1].setVisible(state);
        ui->shadowSpinBox->setEnabled(state);
        ui->zeroShadowSpinBox->setEnabled(state);
        ui->smoothSpinBox->setEnabled(state);
        ui->mulBox->setEnabled(state);
        if(state){
            while(!buttons.isEmpty()){
                buttons.takeLast()->setChecked(false);
            }
            buttons.append(ui->normBox);
            rescale();
        }
    });
    QObject::connect(ui->shadowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setShadowCurrent(val, ui->zeroShadowSpinBox->value(), file, id);
    });
    QObject::connect(ui->zeroShadowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setShadowCurrent(ui->shadowSpinBox->value(), val, file, id);
    });
    QObject::connect(ui->smoothSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        setSmooth(val, file, id);
    });
    QObject::connect(ui->mulBox, &QCheckBox::toggled, this, [=](bool state){
        ui->mulCoeffSpinBox->setEnabled(state);
        ui->linearBox->setEnabled(state);
        ui->particularCurvatureSpinBox->setEnabled(state);
        setNormalizationCoeff(ui->mulCoeffSpinBox->value(), state, file, id);
        if(state){
            buttons.append(ui->mulBox);
            setRelativeCurv(ui->particularCurvatureSpinBox->value(), file, id);
        }else{
            buttons.takeLast()->setChecked(false);
        }
    });
    QObject::connect(ui->mulCoeffSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setNormalizationCoeff(val, ui->mulBox->isChecked(), file, id);
    });
    QObject::connect(ui->particularCurvatureSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setRelativeCurv(val, file, id);
    });
    QObject::connect(ui->linearBox, &QCheckBox::toggled, this, [=](bool state){
        ui->llSpinBox->setEnabled(state);
        ui->rlSpinBox->setEnabled(state);
        ui->linearBackgroundBox->setEnabled(state);
        setLinearIntervals(QPointF(ui->llSpinBox->value(), ui->rlSpinBox->value()), state, file, id);
        line[0].setVisible(state);
        line[1].setVisible(state);
        if(state){
            buttons.append(ui->linearBox);
        }else{
            buttons.takeLast()->setChecked(false);
        }
        rescale();
    });
    QObject::connect(ui->llSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setLinearIntervals(QPointF(val, ui->rlSpinBox->value()), ui->linearBox->isChecked(), file, id);
    });
    QObject::connect(ui->rlSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setLinearIntervals(QPointF(ui->llSpinBox->value(), val), ui->linearBox->isChecked(), file, id);
    });
    QObject::connect(ui->linearBackgroundBox, &QCheckBox::toggled, this, [=](bool state){
        setLin(state, file, id);
        ui->filletSpinBox->setEnabled(state);
        ui->steppedBackgroundBox->setEnabled(state);
        line[0].setVisible(!state);
        line[1].setVisible(!state);
        if(state){
            buttons.append(ui->linearBackgroundBox);
        }else{
            buttons.takeLast()->setChecked(false);
        }
        steps.setVisible(state);
        rescale();
    });
    QObject::connect(ui->filletSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setStepped(val, ui->steppedBackgroundBox->isChecked(), file, id);
    });
    QObject::connect(ui->steppedBackgroundBox, &QCheckBox::toggled, this, [=](bool state){
        setStepped(ui->filletSpinBox->value(), state, file, id);
        ui->diffBox->setEnabled(state);
        if(state){
            buttons.append(ui->steppedBackgroundBox);
        }else{
            buttons.takeLast()->setChecked(false);
        }
        steps.setVisible(!state);
    });
    QObject::connect(ui->diffBox, &QCheckBox::toggled, this, [=](bool state){
        ui->integrationLimitSpinBox->setValue(raw[file].pointsVector().size()/2);
        xmcd.setVisible(state);
        xmcdZero.setVisible(state);
        axisY2.setVisible(state);
        ui->integrateBox->setEnabled(state);
        if(state){
            buttons.append(ui->diffBox);
        }else{
            buttons.takeLast()->setChecked(false);
        }
    });
    QObject::connect(ui->integrateBox, &QCheckBox::toggled, this, [=](bool state){
        ui->integrationLimitSpinBox->setEnabled(state);
        ui->positiveCheckBox->setEnabled(state);
        dot.setVisible(state);
        setIntegrate(state, ui->integrationLimitSpinBox->value(), file, id);
        integrated[file] = state;
        if(integrated[0] && integrated[1]){
            ui->calculateBox->setEnabled(state);
        }else{
            ui->calculateBox->setEnabled(false);
        }
        if(state){
            dot.replace(0, xmcd.at(ui->integrationLimitSpinBox->value()));
            buttons.append(ui->integrateBox);
        }else{
            summLabels[file]->setText("");
            dl2Labels[file]->setText("");
            dl3Labels[file]->setText("");
            mSELabels[file]->setText("");
            mOLabels[file]->setText("");
            relationLabels[file]->setText("");
            buttons.takeLast()->setChecked(false);
        }
    });
    QObject::connect(ui->integrationLimitSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int val){
        dot.replace(0, xmcd.at(ui->integrationLimitSpinBox->value()));
        setIntegrate(ui->integrateBox->isChecked(), val, file, id);
    });
    QObject::connect(ui->positiveCheckBox, &QCheckBox::toggled, this, [=](bool state){
        setPositiveIntegrals(state, file, id);
    });
    QObject::connect(ui->calculateBox, &QCheckBox::toggled, this, [=](bool state){
        qreal phi1 = qDegreesToRadians(theta[0] + ui->phi1OffsetSpinBox->value());
        qreal phi2 = qDegreesToRadians(theta[1] + ui->phi2OffsetSpinBox->value());
        state = state && integrated[0] && integrated[1];
        setCalculate(state, QPointF(phi1, phi2), QPointF(qDegreesToRadians(theta[0]), qDegreesToRadians(theta[1])), id);
        if(state){
            buttons.append(ui->calculateBox);
            pairs.at(id)->state[0].insert("calc", ui->calculateBox->isChecked());
            pairs.at(id)->state[1].insert("calc", ui->calculateBox->isChecked());
        }else{
            ui->mOrbPLabel->setText("");
            ui->mOrbOLabel->setText("");
            ui->msLabel->setText("");
            ui->mTLabel->setText("");
            ui->mOrbLabel->setText("");
            ui->summaryTable->item(id + file, 2)->setText(ui->msLabel->text());
            ui->summaryTable->item(id + file, 3)->setText(ui->mTLabel->text());
            ui->summaryTable->item(id + file, 4)->setText(ui->mOrbPLabel->text());
            ui->summaryTable->item(id + file, 5)->setText(ui->mOrbOLabel->text());
            ui->summaryTable->item(id + file, 6)->setText(ui->mOrbLabel->text());
            buttons.takeLast()->setChecked(false);
        }
    });
    QObject::connect(ui->phi1OffsetSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]{
        qreal phi1 = qDegreesToRadians(theta[0] + ui->phi1OffsetSpinBox->value());
        qreal phi2 = qDegreesToRadians(theta[1] + ui->phi2OffsetSpinBox->value());
        bool state = ui->calculateBox->isChecked() && integrated[0] && integrated[1];
        setCalculate(state, QPointF(phi1, phi2), QPointF(qDegreesToRadians(theta[0]), qDegreesToRadians(theta[1])), id);
        phiLabels[0]->setText(QString::number(theta[0] + offsets[0]->value()));
    });
    QObject::connect(ui->phi2OffsetSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]{
        qreal phi1 = qDegreesToRadians(theta[0] + ui->phi1OffsetSpinBox->value());
        qreal phi2 = qDegreesToRadians(theta[1] + ui->phi2OffsetSpinBox->value());
        bool state = ui->calculateBox->isChecked() && integrated[0] && integrated[1];
        setCalculate(state, QPointF(phi1, phi2), QPointF(qDegreesToRadians(theta[0]), qDegreesToRadians(theta[1])), id);
        phiLabels[1]->setText(QString::number(theta[1] + offsets[1]->value()));
    });
    QObject::connect(ui->pSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setIntegrationConstants(val, ui->nSpinBox->value(), id);
    });
    QObject::connect(ui->nSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](double val){
        setIntegrationConstants(ui->pSpinBox->value(), val, id);
    });
    QObject::connect(ui->exportButton, &QPushButton::pressed, this, [=](){
        if(ui->exportLine->text().size() == 0){
            exportDialog();
        }
        QString exPath = ui->exportLine->text();
        if(!QDir(exPath).exists()){
            QDir().mkdir(exPath);
        }
        if(!QDir(exPath + "/" + QString::number(file)).exists()){
            QDir().mkdir(exPath + "/" + QString::number(file));
        }
        QList<QtCharts::QAbstractSeries*>::iterator end = chart.series().end()--;
        std::for_each(chart.series().begin(), end, [=](QtCharts::QAbstractSeries* const ser){
            if(ser->isVisible()){
                QtCharts::QLineSeries *lineSer;
                if(ser && (ser != &xmcdZero) && (ser != &line[0]) && (ser != &line[1])){
                    lineSer = dynamic_cast<QtCharts::QLineSeries *>(ser);
                    if(lineSer){
                        QFile outFile(exPath + "/" + QString::number(file) + "/" + lineSer->name() + ".txt");
                        outFile.open(QIODevice::WriteOnly);
                        QTextStream outStream(&outFile);
                        QVector<QPointF> tmp = lineSer->pointsVector();
                        for(int i = 0; i < tmp.size(); ++i){
                            outStream << tmp[i].x() << "    " << tmp[i].y() << "\n";
                        }
                        outFile.close();

                    }else{
                        log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + "unable to cast to QLineSeries");
                    }
                }
            }
        });
    });
    QObject::connect(ui->bSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{
        setLimits(ui->bSpinBox->value(), ui->tSpinBox->value(), file, id);
    });
    QObject::connect(ui->tSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{
        setLimits(ui->bSpinBox->value(), ui->tSpinBox->value(), file, id);
    });
    QObject::connect(ui->smoothSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i){
        setSmooth(i, file, id);
    });
    QObject::connect(ui->shadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]{
        setShadowCurrent(ui->shadowSpinBox->value(), ui->zeroShadowSpinBox->value(), file, id);
    });
    QObject::connect(ui->zeroShadowSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]{
        setShadowCurrent(ui->shadowSpinBox->value(), ui->zeroShadowSpinBox->value(), file, id);
    });
    QObject::connect(ui->swapButton, &QPushButton::pressed, this, [=](){
        if(file == 0){
            pairs[id]->ui.file2Button->setChecked(true);
        }else{
            pairs[id]->ui.file1Button->setChecked(true);
        }
    });
    QObject::connect(ui->setExportPathButton, &QPushButton::pressed, this, &MainWindow::exportDialog);
    QObject::connect(ui->setExportPath2Button, &QPushButton::pressed, this, &MainWindow::exportDialog);
    QObject::connect(ui->selectPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select data directory"), dataDir);
        if(dir.length() != 0){
            refresh->removePath(dataDir);
            dataDir = dir;
            open(dir);
            refresh->addPath(dir);
        }
    });
    QObject::connect(refresh, &QFileSystemWatcher::directoryChanged, this, &MainWindow::open);
    QObject::connect(ui->pairButton, &QPushButton::pressed, this, [=](){
        if(fileName[0].size() != 0 && fileName[1].size() != 0){
            pairs.append(new PairWidget());
            wrapper->appendCalc();
            for(int i = 0; i < 2; ++i){
                dataPointers[i].append(nullptr);
                rawPointers[i].append(nullptr);
                zeroPointers[i].append(nullptr);
                stepPointers[i].append(nullptr);
                xmcdPointers[i].append(nullptr);
            }
            int id = pairs.count() - 1;
            ui->pairTable->insertRow(id);
            ui->pairTable->setRowHeight(id, pairs.last()->height());
            ui->pairTable->setCellWidget(id, 0, pairs.last());
            for(int file = 0; file < 2 ; ++file){
                fileNameLabel[file]->setText("");
                pairs.last()->state[file] = defaults();
                pairs.last()->fileLabels[file]->setText(fileName[file]);
                pairs.last()->state[file].insert("filename", filePath[file] + "/" + fileName[file]);
                if(holderBox[file]->isChecked() && (theta[file] <= 2) && (theta[file] >= 0)){
                    pairs.last()->state[file].insert("theta", angle[(int)theta[file]]);
                }else{
                    pairs.last()->state[file].insert("theta", theta[file]);
                }
                pairs.last()->state[file].insert("loaded", true);
                pairs.last()->state[file].insert("sample", sample);
                pairs.last()->state[file].insert("energy", energy);
                fileName[file] = "";
                filePath[file] = "";
            }
            pairs.last()->ui.groupBox->setTitle(sample + " " + energy + " " +
                                                QString::number(pairs.last()->state[0].value("theta", -1.0).toDouble()) + "/" +
                                                QString::number(pairs.last()->state[1].value("theta", -1.0).toDouble()));
            pairs.at(id)->fileButtons[1]->setChecked(true);
            ui->file1Button->setChecked(true);
            QObject::connect(pairs.last(), &PairWidget::selected, this, [=]{
                selectPair(pairs.indexOf(dynamic_cast<PairWidget *>(sender())));
            });
            QObject::connect(pairs.last(), &PairWidget::deletePressed, this, [=]{
                int id = pairs.indexOf(dynamic_cast<PairWidget *>(sender()));
                PairWidget *curr = pairs.at(id);
                ui->pairTable->removeRow(id);
                if(pairs.size() > 1){
                    pairs.removeAt(id);
                    if(id == this->id){
                        if(id != 0){
                            selectPair(id - 1);
                        }else{
                            selectPair(0);
                        }
                    }
                }else{
                    ui->swapButton->setEnabled(false);
                    ui->rawBox->setEnabled(false);
                    ui->zeroBox->setEnabled(false);
                    ui->normBox->setEnabled(false);
                    pairs.removeAt(id);
                    this->id = -1;
                    file = 0;
                    loadState(defaults());
                }
                ui->summaryTable->removeRow(id);
                ui->summaryTable->removeRow(id + 1);
                wrapper->removeCalc(id);
                updateSummary();
                delete curr;
            });
            QObject::connect(pairs.last(), &PairWidget::fileSelected, this, [=](int file){
                this->file = file;
                loadState(pairs.at(id)->state[file]);
                chart.setTitle(pairs.last()->state[file].value("sample", "null").toString() + " " +
                        pairs.last()->state[file].value("energy", "null").toString() + " " +
                        QString::number(pairs.last()->state[file].value("theta", -1.0).toDouble()));
                update(file, id);
            });
            integrated[0] = false;
            integrated[1] = false;
            selectPair(id);
            ui->summaryTable->insertRow(id);
            ui->summaryTable->insertRow(id + 1);
            QCheckBox* use = new QCheckBox(pairs.last());
            QDoubleSpinBox* xOffset = new QDoubleSpinBox(pairs.last());
            QDoubleSpinBox* yOffset = new QDoubleSpinBox(pairs.last());
            use->setChecked(true);
            //use->setMaximumWidth(10);
            use->connect(use, &QCheckBox::toggled, this, [=](bool state){
               xOffset->setEnabled(state);
               yOffset->setEnabled(state);
               updateSummary();
            });
            ui->summaryTable->setCellWidget(id, 0, use);
            ui->summaryTable->setSpan(id, 0, 2, 1);
            ui->summaryTable->setItem(id, 1, new QTableWidgetItem(sample + " " + energy + " " +
                                                                  QString::number(pairs.last()->state[0].value("theta",
                                                                                                               -1.0).toDouble())));
            ui->summaryTable->setItem(id + 1, 1, new QTableWidgetItem(sample + " " + energy + " " +
                                                                  QString::number(pairs.last()->state[1].value("theta",
                                                                                                               -1.0).toDouble())));
            ui->summaryTable->setItem(id, 2, new QTableWidgetItem("mS/mO"));
            ui->summaryTable->setItem(id + 1, 2, new QTableWidgetItem("mS/mO"));
            ui->summaryTable->setItem(id, 3, new QTableWidgetItem("mS"));
            ui->summaryTable->setSpan(id, 3, 2, 1);
            ui->summaryTable->setItem(id, 4, new QTableWidgetItem("mT"));
            ui->summaryTable->setSpan(id, 4, 2, 1);
            ui->summaryTable->setItem(id, 5, new QTableWidgetItem("mOrb∥"));
            ui->summaryTable->setSpan(id, 5, 2, 1);
            ui->summaryTable->setItem(id, 6, new QTableWidgetItem("mOrb⊥"));
            ui->summaryTable->setSpan(id, 6, 2, 1);
            ui->summaryTable->setItem(id, 7, new QTableWidgetItem("mOrb"));
            ui->summaryTable->setSpan(id, 7, 2, 1);
            xOffset->setMinimum(-10.0);
            xOffset->setMaximum(10.0);
            xOffset->setSingleStep(0.1);
            xOffset->connect(xOffset, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](){
                log("xOffset changed");
                updateSummary();
            });
            ui->summaryTable->setCellWidget(id, 8, xOffset);
            ui->summaryTable->setSpan(id, 8, 2, 1);
            yOffset->setMinimum(-10.0);
            yOffset->setMaximum(10.0);
            yOffset->setSingleStep(0.1);
            yOffset->connect(yOffset, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](){
                log("yOffset changed");
                updateSummary();
            });
            ui->summaryTable->setCellWidget(id, 9, yOffset);
            ui->summaryTable->setSpan(id, 9, 2, 1);
            emit ui->swapButton->pressed();
        }
    });
    QObject::connect(ui->summaryButtonGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), this, [=](bool state){
        if(state){//wtf?
            updateSummary();
        }
    });
    loadSettings();
    ui->pairTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->pairTable->setColumnCount(1);
    ui->pairTable->horizontalHeader()->setHidden(true);
    ui->pairTable->setColumnWidth(0, 220);
    calcThread->start();
    if(session->value("normalExit", false).toBool()){
        loadState(defaults());
    }else{
        log("crash detected");
        loadState(defaults());//debug
        //load all nods
        //loadState();
        //clear unused ini files
    }
    session->setValue("normalExit", false);
    ui->sessionButton->setEnabled(false); //disabled in current version
    ui->unitBox->setEnabled(false); //same
}

MainWindow::~MainWindow()
{
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    delete refresh;
    calcThread->quit();
    calcThread->requestInterruption();
    if(calcThread->wait()){
    }else{
        log("thread exit error");
    }
    delete calcThread;
    saveSettings();
    delete table;
    session->setValue("normalExit", true);
    delete session;
    delete ui;
}

void MainWindow::exportDialog(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select export directory"), dataDir);
    if(dir.length() != 0){
        ui->exportLine->setText(dir);
        ui->summaryPathLineEdit->setText(dir);
    }
}

void MainWindow::selectPair(const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + QString::number(id));
    if(this->id != id && id >= 0){
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
}

void MainWindow::rescale(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    QPointF x(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QPointF y(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QtCharts::QLineSeries *lineSer;
    QList<QtCharts::QAbstractSeries*>::iterator end = chart.series().end()--;
    std::for_each(chart.series().begin(), end, [&x, &y, &lineSer, this](QtCharts::QAbstractSeries* const ser){//"this" for debug
        if(ser->isVisible()){
            if(ser && ser != &dot && ser != &xmcd && ser != &xmcdZero){
                lineSer = dynamic_cast<QtCharts::QLineSeries *>(ser);
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
                    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__  + " unable to cast to QLineSeries");
                }
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

void MainWindow::loadSettings(){
    QSettings settings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.beginGroup("gui");
        resize(settings.value("windowSize", QSize(800, 600)).toSize());
        move(settings.value("windowPosition", QPoint(0, 0)).toPoint());
        setWindowTitle(settings.value("windowTitle", "XMCDAnalyzer(corrupted .ini)").toString());
        dataDir = settings.value("lastDir", "").toString();
        settings.beginGroup("fileTable");
            int columnCount = settings.beginReadArray("columns");
            for(int i = 0; i < columnCount; ++i){
                settings.setArrayIndex(i);
                ui->fileTable->setColumnWidth(i, settings.value("width", 20). toInt());
            }
            settings.endArray();
        settings.endGroup();
        settings.beginGroup("summaryTable");
            columnCount = settings.beginReadArray("columns");
            for(int i = 0; i < columnCount; ++i){
                settings.setArrayIndex(i);
                ui->summaryTable->setColumnWidth(i, settings.value("width", 20). toInt());
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
    settings.beginGroup("defaultValues");
        ui->nSpinBox->setValue(settings.value("nh", 2.49).toDouble());
        ui->pSpinBox->setValue(settings.value("pc", 0.7).toDouble());
    settings.endGroup();
    settings.beginGroup("logging");
        logEnabled = settings.value("enabled", false).toBool();
        logPath = settings.value("path", "").toString();
        useExeLocation = settings.value("useExeLocation", true).toBool();
        debugMode = settings.value("debug", false).toBool();
        debugCopy = settings.value("copy", false).toBool();
        if(logEnabled){
            if(debugMode){
                logger = new Logger("", debugCopy, this);
            }else{
                if(useExeLocation){
                    logger = new Logger(QApplication::applicationDirPath(), debugCopy, this);
                }else{
                    logger = new Logger(logPath, debugCopy, this);
                }
            }
            QObject::connect(this, &MainWindow::log, logger, &Logger::print);
        }
    settings.endGroup();
}

void MainWindow::saveSettings(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    QSettings settings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.clear();
    settings.beginGroup("gui");
        settings.setValue("windowSize", this->size());
        settings.setValue("windowPosition", this->pos());
        settings.setValue("windowTitle", this->windowTitle());
        settings.setValue("lastDir", dataDir);
        settings.beginGroup("fileTable");
            settings.beginWriteArray("columns");
            for(int i = 0; i < table->columnCount(QModelIndex()); ++i){
                        settings.setArrayIndex(i);
                        settings.setValue("width", ui->fileTable->columnWidth(i));
                    }
            settings.endArray();
        settings.endGroup();
        settings.beginGroup("summaryTable");
            settings.beginWriteArray("columns");
            for(int i = 0; i < ui->summaryTable->columnCount(); ++i){
                    settings.setArrayIndex(i);
                    settings.setValue("width", ui->summaryTable->columnWidth(i));
                }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
    settings.beginGroup("defaultValues");
        settings.setValue("nh", ui->nSpinBox->value());
        settings.setValue("pc", ui->pSpinBox->value());
    settings.endGroup();
    settings.beginGroup("logging");
        settings.setValue("enabled", logEnabled);
        settings.setValue("path", logPath);
        settings.setValue("useExeLocation", useExeLocation);
        settings.setValue("debug", debugMode);
        settings.setValue("copy", debugCopy);
        if(logEnabled){
            log("settings saved");
            delete logger;
        }
    settings.endGroup();
}

void MainWindow::open(QString selectedPath){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + " " + selectedPath);
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
            if(ui->geomBox->isChecked() && ((par.value("angle", "-1").toString() == "-1") ||
                                            (par.value("angle", "-1").toString() == "") ||
                                            (par.value("angle", "noInfo").toString() == geom))){
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

void MainWindow::loadState(const QHash<QString, QVariant> state){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    bool loaded = state.value("loaded", false).toBool();
    if(id >= 0){
        setIntegrationConstants(ui->pSpinBox->value(), ui->nSpinBox->value(), id);
    }else{
        //reset gui only
    }
    if(loaded){
        fileName[file] = state.value("filename", "").toString();
        pairs.at(id)->fileButtons[file]->setChecked(true);
        setLoader(fileName[file], file, id);
        theta[file] = state.value("theta", 0.0).toDouble();
        thetaLabels[file]->setText(QString::number(theta[file]));
        offsets[file]->setEnabled(true);
        ui->swapButton->setEnabled(true);
        ui->rawBox->setEnabled(true);
        ui->zeroBox->setEnabled(true);
        ui->normBox->setEnabled(true);
    }
    ui->bSpinBox->setEnabled(loaded);
    ui->tSpinBox->setEnabled(loaded);
    if(state.value("isRaw", false).toBool()){
        ui->zeroBox->setChecked(true);
        ui->rawBox->setChecked(true);
    }else if(state.value("isZero", false).toBool()){
        ui->rawBox->setChecked(true);
        ui->zeroBox->setChecked(true);
    }else{
        ui->rawBox->setChecked(true);
        ui->normBox->setChecked(true);
    }
    ui->shadowSpinBox->setValue(state.value("shadow", 0.0).toDouble());
    ui->zeroShadowSpinBox->setValue(state.value("zeroShadow", 0.0).toDouble());
    ui->mulBox->setChecked(state.value("isMult", false).toBool());
    ui->mulCoeffSpinBox->setValue(state.value("mulCoeff", 0.0).toDouble());
    ui->particularCurvatureSpinBox->setValue(state.value("relativeCurv", 0.0).toDouble());
    ui->smoothSpinBox->setValue(state.value("smooth", 1).toInt());
    ui->bSpinBox->setValue(state.value("cropL", 0).toInt());
    ui->tSpinBox->setValue(state.value("cropR", 0).toInt());
    ui->linearBox->setChecked(state.value("linearShow", false).toBool());
    ui->llSpinBox->setValue(state.value("linearL", 2.0).toDouble());
    ui->rlSpinBox->setValue(state.value("linearR", 2.0).toDouble());
    ui->filletSpinBox->setValue(state.value("steppedMul", 1.0).toDouble());
    ui->linearBackgroundBox->setChecked(state.value("linear", false).toBool());
    ui->steppedBackgroundBox->setChecked(state.value("stepped", false).toBool());
    ui->diffBox->setChecked(state.value("xmcd", false).toBool());
    ui->integrateBox->setChecked(state.value("integrate", false).toBool());
    ui->integrationLimitSpinBox->setValue(state.value("integrationLimit", 0).toInt());
    ui->positiveCheckBox->setChecked(state.value("positiveIntegrals", false).toBool());
    if(loaded){
        offsets[file]->setValue(state.value("phiOff", 0.0).toDouble());
    }else{
        for(int i = 0; i < 2; ++i){
            offsets[i]->setValue(0);
        }
    }
    phiLabels[file]->setText(QString::number(theta[file] + offsets[file]->value()));
    ui->calculateBox->setChecked(state.value("calc", false).toBool());
    rescale();
}


QHash<QString, QVariant> MainWindow::defaults(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    QHash<QString, QVariant> tmp;
    tmp.insert("loaded", false);
    tmp.insert("isRaw", false);
    tmp.insert("isZero", false);
    tmp.insert("isNorm", true);
    tmp.insert("shadow", 0.0);
    tmp.insert("zeroShadow", 0.0);
    tmp.insert("isMult", false);
    tmp.insert("mulCoeff", 0.0);
    tmp.insert("relativeCurv", 0.0);
    tmp.insert("smooth", 1);
    tmp.insert("cropL", 0);
    tmp.insert("cropR", 0);
    tmp.insert("linearShow", false);
    tmp.insert("linearL", 2.0);
    tmp.insert("linearR", 2.0);
    tmp.insert("steppedMul", 1.0);
    tmp.insert("linear", false);
    tmp.insert("stepped", false);
    tmp.insert("xmcd", false);
    tmp.insert("integrationLimit", 0);
    tmp.insert("integrate", false);
    tmp.insert("positiveIntegrals", false);
    tmp.insert("calc", false);
    tmp.insert("phiOff", 0.0);
    tmp.insert("theta", 0.0);
    tmp.insert("sample", "null");
    tmp.insert("energy", "null");
    return tmp;
}

void MainWindow::updateSummary(){ //mb make it faster, parsing id of the changed row
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    summaryChart.removeAllSeries();
    qreal xMin = std::numeric_limits<double>::max();
    qreal yMin = std::numeric_limits<double>::max();
    qreal xMax = std::numeric_limits<double>::min();
    qreal yMax = std::numeric_limits<double>::min();
    bool badNormalization = false;
    for(int i = 0; i < (int)(ui->summaryTable->rowCount()/2); ++i){
        if(dynamic_cast<QCheckBox *>(ui->summaryTable->cellWidget(i, 0))->isChecked()){
            for(int j = 0; j < 2; ++j){
                qreal xOff = dynamic_cast<QDoubleSpinBox *>(ui->summaryTable->cellWidget(i, 8))->value();
                qreal yOff = dynamic_cast<QDoubleSpinBox *>(ui->summaryTable->cellWidget(i, 9))->value();
                qreal mult = 1.0;
                if(ui->summaryButtonGroup->checkedId() != 3){
                    QVector<QPair<qreal, QPointF>> const * data;
                    bool ok = true;
                    switch (ui->summaryButtonGroup->checkedId()) {
                    case 0:
                        yOff = yOff*rawOffsetMult;
                        data = rawPointers[j].at(i);
                        break;
                    case 1:
                        yOff = yOff*zeroOffsetMult;
                        data = zeroPointers[j].at(i);
                        break;
                    case 2:
                        yOff = yOff*normOffsetMult;
                        data = dataPointers[j].at(i);
                        if(stepPointers[j].at(i) != nullptr){
                            mult = stepSize/(stepPointers[j].at(i)->last().y() - stepPointers[j].at(i)->first().y());
                            ui->summaryTable->item(i + j, 2)->setBackgroundColor(Qt::white);
                        }else{
                            badNormalization = true;
                            ui->summaryTable->item(i + j, 2)->setBackgroundColor(Qt::red);
                        }
                        break;
                    default:
                        ok = false;
                        log(QString(this->metaObject()->className()) + "::" +  __FUNCTION__  + "unknown button id");
                        break;
                    }
                    if(ok && (data != nullptr)){
                        QtCharts::QLineSeries* line = new QtCharts::QLineSeries(ui->summaryChartWidget);
                        line->setName(ui->summaryTable->item(i + j, 1)->text());
                        for(int k = 0; k < data->size(); ++k){
                            line->append(data->at(k).first + xOff, data->at(k).second.x()*mult + yOff);
                            if(xMin > line->points().last().x()){
                                xMin = line->points().last().x();
                            }else if(xMax < line->points().last().x()){
                                xMax = line->points().last().x();
                            }
                            if(yMin > line->points().last().y()){
                                yMin = line->points().last().y();
                            }else if(yMax < line->points().last().y()){
                                yMax = line->points().last().y();
                            }
                        }
                        for(int k = data->size() - 1; k > -1; --k){
                            line->append(data->at(k).first + xOff, data->at(k).second.y()*mult + yOff);
                            if(yMin > line->points().last().y()){
                                yMin = line->points().last().y();
                            }else if(yMax < line->points().last().y()){
                                yMax = line->points().last().y();
                            }
                        }
                        summaryChart.addSeries(line);
                        line->attachAxis(&summaryAxisX);
                        line->attachAxis(&summaryAxisY);
                        ui->summaryTable->item(i + j, 1)->setBackgroundColor(line->color());
                    }else{
                        //calc thread still working
                    }
                }else{
                    QVector<QPointF> const * data = xmcdPointers[j].at(i);
                    if(data != nullptr){
                        if(stepPointers[j].at(i) != nullptr){
                            mult = stepSize/(stepPointers[j].at(i)->last().y() - stepPointers[j].at(i)->first().y());
                            ui->summaryTable->item(i + j, 2)->setBackgroundColor(Qt::white);
                        }else{
                            badNormalization = true;
                            ui->summaryTable->item(i + j, 2)->setBackgroundColor(Qt::red);
                        }
                        QtCharts::QLineSeries* line = new QtCharts::QLineSeries(ui->summaryChartWidget);
                        line->setName(ui->summaryTable->item(i + j, 1)->text());
                        for(int k = 0; k < data->size(); ++k){
                            line->append(data->at(k).x() + xOff, data->at(k).y()*mult + yOff);
                            if(xMin > line->points().last().x()){
                                xMin = line->points().last().x();
                            }else if(xMax < line->points().last().x()){
                                xMax = line->points().last().x();
                            }
                            if(yMin > line->points().last().y()){
                                yMin = line->points().last().y();
                            }else if(yMax < line->points().last().y()){
                                yMax = line->points().last().y();
                            }
                        }
                        summaryChart.addSeries(line);
                        line->attachAxis(&summaryAxisX);
                        line->attachAxis(&summaryAxisY);
                        ui->summaryTable->item(i + j, 1)->setBackgroundColor(line->color());
                    }else{
                        //calc thread still working
                    }
                }
            }
        }else{
            ui->summaryTable->item(i, 1)->setBackgroundColor(Qt::white);
            ui->summaryTable->item(i + 1, 1)->setBackgroundColor(Qt::white);
        }
    }
    if(badNormalization){
        ui->summaryNormButton->setStyleSheet("background-color: red;");
    }else{
        ui->summaryNormButton->setStyleSheet("background-color: white;"); //fix white box
    }
    summaryAxisX.setRange(xMin, xMax);
    summaryAxisY.setRange(yMin, yMax);
}
