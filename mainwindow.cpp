#include "mainwindow.h"
#include <QDateTime>
#include <QtMath>
#include <QStorageInfo>
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    /*To-do list:
     *
     *XMCD does not fit chart
     * crash at swap with calculateBox checked
     *
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
    summRelLabels[0] = ui->summ1AreaRelLabel;
    summRelLabels[1] = ui->summ2AreaRelLabel;
    dl2RelLabels[0] = ui->l21AreaRelLabel;
    dl2RelLabels[1] = ui->l22AreaRelLabel;
    dl3RelLabels[0] = ui->l31AreaRelLabel;
    dl3RelLabels[1] = ui->l32AreaRelLabel;
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
    ui->summaryButtonGroup->setId(ui->summaryHalfSumButton, 4);
    refresh = new QFileSystemWatcher(this);
    ui->summaryTable->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList summaryLabels;
    summaryLabels.append("Use");
    summaryLabels.append("Name");
    summaryLabels.append("Signal");
    summaryLabels.append("mS/mO");
    summaryLabels.append("mS");
    summaryLabels.append("mT");
    summaryLabels.append("mOrb∥");
    summaryLabels.append("mOrb⊥");
    summaryLabels.append("mOrb");
    summaryLabels.append("E. shift");
    summaryLabels.append("I. shift");
    summaryLabels.append("Mult.");
    ui->summaryTable->setColumnCount(summaryLabels.size());
    ui->summaryTable->setHorizontalHeaderLabels(summaryLabels);
    ui->tabWidget->setCurrentIndex(0);
    chart.setTitle("XAS");
    chart.setAxisX(&axisX);
    chart.addAxis(&axisY, Qt::AlignLeft);
    chart.addAxis(&axisY2, Qt::AlignRight);
    axisY.setGridLineColor(QColor(150, 250 , 150));
    axisY.setLinePenColor(QColor(0, 150 , 0));
    axisY.setLabelFormat("%2.2e");
    axisY.setMinorGridLineVisible(false);
    axisY2.setGridLineColor(QColor(150, 150 , 250));
    axisY2.setLinePenColor(QColor(0, 0 , 150));
    axisY2.setLabelFormat("%2.2e");
    axisY2.setMinorGridLineVisible(false);
    axisX.setMinorGridLineVisible(true);
    axisX.setMinorTickCount(4);
    axisX.setTickCount(20);
    axisX.setTitleText("primary photons energy, eV");
    axisY.setMinorTickCount(1);
    axisY.setTickCount(10);
    axisY.setTitleText("Intensity XAS, arb.u.");
    axisY2.setMinorTickCount(1);
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
        refData[file].setName("Ref." + QString::number(file));
        chart.addSeries(&refData[file]);
        refData[file].attachAxis(&axisX);
        refData[file].attachAxis(&axisY);
        refData[file].setVisible(false);
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
    refxmcd.setName("Ref XMCD");
    chart.addSeries(&refxmcd);
    refxmcd.attachAxis(&axisX);
    refxmcd.attachAxis(&axisY2);
    refxmcd.setVisible(false);
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
    summaryAxisY.setMinorTickCount(1);
    summaryAxisY.setTickCount(11);
    summaryAxisY.setTitleText("Intensity, arb.u.");
    summaryAxisY.setMinorGridLineVisible(false);
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
    QObject::connect(this, &MainWindow::setEnergyShift, wrapper, &CalcWrapper::setEnergyShift, Qt::QueuedConnection);
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
    QObject::connect(this, &MainWindow::setGround, wrapper, &CalcWrapper::setGround, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setRelativeCurv, wrapper, &CalcWrapper::setRelativeCurv, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::activateRef, wrapper, &CalcWrapper::activateRef, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::setArea, wrapper, &CalcWrapper::setArea, Qt::QueuedConnection);

    //wrapper to this
    QObject::connect(wrapper, &CalcWrapper::log, this, [=](QVariant out){
        emit log("wrapper:: " + out.toString());
    });
    QObject::connect(wrapper, &CalcWrapper::setOffset, this, [=](bool state){
        emit setOffset(state);
    });
    QObject::connect(wrapper, &CalcWrapper::processedData, this, [=](const QVector<QPair<qreal, QPointF>>* points,
                     const int id, const int file, const int ref){
        log(QString(this->metaObject()->className()) + "<- processedData:: id = "  + QString::number(id) +
            ". file = " + QString::number(file) + ". ref = " + QString::number(ref));
        setOffset(true);
        if(ref == 0){
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
        }else if(ref == 1 && ui->refCheckBox->isChecked()){
            qreal mult = 1.0;
            qreal max = std::numeric_limits<double>::min();
            for(int i = 0; i < dataPointers[file].at(id)->size(); ++i){
                qreal hs = (dataPointers[file].at(id)->at(i).second.x() + dataPointers[file].at(id)->at(i).second.y())/2;
                if(hs > max){
                    max = hs;
                }
            }
            qreal refMax = std::numeric_limits<double>::min();
            for(int i = 0; i < points->size(); ++i){
                qreal hs = (points->at(i).second.x() + points->at(i).second.y())/2;
                if(hs > refMax){
                    refMax = hs;
                }
            }
            mult = max/refMax;
            pairs.at(id)->state[file].insert("refMult", mult);
            refData[0].blockSignals(true);
            refData[0].clear();
            refData[1].blockSignals(true);
            refData[1].clear();
            for(int i = 0; i < points->size(); ++i){
                refData[0].append(points->at(i).first, points->at(i).second.x()*mult);
                refData[1].append(points->at(i).first, points->at(i).second.y()*mult);
            }
            refDataPointers[file].replace(id, points);
            refData[0].blockSignals(false);
            refData[1].blockSignals(false);
            refData[0].pointsReplaced();
            refData[1].pointsReplaced();

        }else{
            log("Error: " + QString(this->metaObject()->className()) + "<- processedData:: ref = " + QString::number(ref));
        }
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::rawData, this, [=](const QVector<QPair<qreal,QPointF>>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- rawData:: id = "  + QString::number(id) +
            ". file = " + QString::number(file));
        setOffset(true);
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
        if(id == this->id && this->file == file){
            ui->smoothSpinBox->setMaximum(points->size());
            ui->llSpinBox->setMaximum(points->size());
            ui->rlSpinBox->setMaximum(points->size());
            ui->integrationLimitSpinBox->setMaximum(points->size() - 2);
            ui->integrationLimitSpinBox->setValue(qFloor(points->size()/2));
        }
        raw[0].pointsReplaced();
        raw[1].pointsReplaced();
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::iZero, this, [=](const QVector<QPair<qreal,QPointF>>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- iZeroData:: id = "  + QString::number(id) +
            ". file = " + QString::number(file));
        setOffset(true);
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
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::stepData, this, [=](const QVector<QPointF>* points, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- stepData:: id = "  + QString::number(id) +
            ". file = " + QString::number(file));
        setOffset(true);
        steps.blockSignals(true);
        steps.clear();
        for(int i = 0; i < points->size(); ++i){
            steps.append(points->at(i).x(), points->at(i).y());
        }
        stepPointers[file].replace(id, points);
        steps.blockSignals(false);
        steps.pointsReplaced();
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::XMCD, this, [=](const QVector<QPointF>* points, const int id, const int file, const int ref){
        log(QString(this->metaObject()->className()) + "<- XMCD:: id = "  + QString::number(id) +
            ". file = " + QString::number(file) + ". ref = " + QString::number(ref));
        setOffset(true);
        qreal min = std::numeric_limits<double>::max();
        qreal max = std::numeric_limits<double>::min();
        if(ref == 0){
            xmcd.blockSignals(true);
            xmcd.clear();
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
        }else if(ref == 1){
            refxmcd.blockSignals(true);
            refxmcd.clear();
            qreal mult = pairs.at(id)->state[file].value("refMult", 1.0).toDouble();
            for(int i = 0; i < points->size(); ++i){
                refxmcd.append(points->at(i).x(), points->at(i).y()*mult);
                if(min > points->at(i).y()){
                    min = points->at(i).y();
                }else if(max < points->at(i).y()){
                    max = points->at(i).y();
                }
            }
            refXmcdPointers[file].replace(id, points);
            refxmcd.blockSignals(false);
            refxmcd.pointsReplaced();
            qreal scale = max - min;
            min -= scale * 0.02;
            max += scale * 0.02;
        }else{
            log("Error: " + QString(this->metaObject()->className()) + "<- XMCD:: ref = " + QString::number(ref));
            min = axisY2.min();
            max = axisY2.max();
        }

        if(min != axisY2.min() && max != axisY2.max()){
            axisY2.setRange(min, max);
        }else if(min != axisY2.min()){
            axisY2.setMin(min);
        }else if(max != axisY2.max()){
            axisY2.setMax(max);
        }
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::linCoeffs, this, [=](const QPointF* left, const QPointF* right,
                     const QPointF* x, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- linCoeffs:: id = "  + QString::number(id) +
            ". file = " + QString::number(file));
        setOffset(true);
        Q_UNUSED(x);
        line[0].blockSignals(true);
        line[0].clear();
        line[1].blockSignals(true);
        line[1].clear();
        line[0].append(norm[0].at(0).x(), norm[0].at(0).x() * left->x() + left->y());
        line[0].append(norm[0].at(norm[0].points().size() - 1).x(),
                norm[0].at(norm[0].points().size() - 1).x() * left->x() + left->y());
        line[1].append(norm[0].at(0).x(), norm[0].at(0).x() * right->x() + right->y());
        line[1].append(norm[0].at(norm[0].points().size() - 1).x(),
                norm[0].at(norm[0].points().size() - 1).x() * right->x() + right->y());
        line[0].blockSignals(false);
        line[1].blockSignals(false);
        ui->lkLabel->setText(QString::number(left->x()));
        ui->rkLabel->setText(QString::number(right->x()));
        line[0].pointsReplaced();
        line[1].pointsReplaced();
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::integrals, this, [=](const qreal* summ, const qreal* dl2, const qreal* dl3,
                     const qreal* mSE, const qreal* mO, const qreal* relation, const int id, const int file, const int ref){
        log(QString(this->metaObject()->className()) + "<- integrals:: id = "  + QString::number(id) +
            ". file = " + QString::number(file) + ". ref = " + QString::number(ref));
        setOffset(true);
        if(ref == 0){
            pairs.at(id)->state[file].insert("summArea", *summ);
            pairs.at(id)->state[file].insert("l2Area", *dl2);
            pairs.at(id)->state[file].insert("l3Area", *dl3);
            summLabels[file]->setText(QString::number(*summ));
            dl2Labels[file]->setText(QString::number(*dl2));
            dl3Labels[file]->setText(QString::number(*dl3));
            mSELabels[file]->setText(QString::number(*mSE));
            mOLabels[file]->setText(QString::number(*mO));
            relationLabels[file]->setText(QString::number(*relation));
            ui->summaryTable->item(2*id + file, 2)->setText(relationLabels[file]->text());
        }else if(ref == 1){
            pairs.at(id)->state[file].insert("refSummArea", *summ);
            emit setArea(ui->useRefCheckBox->isChecked(), *summ, file, id);
            pairs.at(id)->state[file].insert("refl2Area", *dl2);
            pairs.at(id)->state[file].insert("refl3Area", *dl3);
        }else{
            log("Error: " + QString(this->metaObject()->className()) + "<- integrals:: ref = " + QString::number(ref));
        }
        if(ui->refCheckBox->isChecked()){
            summRelLabels[file]->setText(QString::number(pairs.at(id)->state[file].value("summArea", 1.0).toDouble()/
                                                         pairs.at(id)->state[file].value("refSummArea", 1.0).toDouble()));
            dl2RelLabels[file]->setText(QString::number(pairs.at(id)->state[file].value("l2Area", 1.0).toDouble()/
                                                         pairs.at(id)->state[file].value("refl2Area", 1.0).toDouble()));
            dl3RelLabels[file]->setText(QString::number(pairs.at(id)->state[file].value("l3Area", 1.0).toDouble()/
                                                         pairs.at(id)->state[file].value("refl3Area", 1.0).toDouble()));
        }else{
            summRelLabels[file]->setText("");
            dl2RelLabels[file]->setText("");
            dl3RelLabels[file]->setText("");
        }
        setOffset();
    });
    QObject::connect(wrapper, &CalcWrapper::moments, this, [=](const qreal* mOP, const qreal* mOO,
                     const qreal* ms, const qreal* mt, const int id, const int file){
        log(QString(this->metaObject()->className()) + "<- moments:: id = "  + QString::number(id) +
            ". file = " + QString::number(file));
        setOffset(true);
        ui->mOrbPLabel->setText(QString::number(*mOP));
        ui->mOrbOLabel->setText(QString::number(*mOO));
        ui->msLabel->setText(QString::number(*ms));
        ui->mTLabel->setText(QString::number(*mt));
        ui->mOrbLabel->setText(QString::number(qSqrt(qPow(*mOP, 2) + qPow(*mOO, 2))));
        ui->summaryTable->item(2*id, 3)->setText(ui->msLabel->text());
        ui->summaryTable->item(2*id, 4)->setText(ui->mTLabel->text());
        ui->summaryTable->item(2*id, 5)->setText(ui->mOrbPLabel->text());
        ui->summaryTable->item(2*id, 6)->setText(ui->mOrbOLabel->text());
        ui->summaryTable->item(2*id, 7)->setText(ui->mOrbLabel->text());
        setOffset();
    }, Qt::QueuedConnection);
    QObject::connect(wrapper, &CalcWrapper::completed, this, [=](const int id, const int file, const int ref){
        log(QString(this->metaObject()->className()) + "<- completed:: id = "  + QString::number(id) +
            ". file = " + QString::number(file) + ". ref = " + QString::number(ref));
        setOffset(true);
        if(file == this->file && id == this->id){
            if(ref == 0){
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
                    pairs.at(id)->state[file].insert("ground", ui->groundCheckBox->isChecked());
                    pairs.at(id)->state[file].insert("calc", ui->calculateBox->isChecked());
                    pairs.at(id)->state[file].insert("phiOff", offsets[file]->value());
                    pairs.at(id)->state[file].insert("useRef", ui->useRefCheckBox->isChecked());
                }
            }else{
                pairs.at(id)->state[file].insert("ref", ui->refCheckBox->isChecked());
                pairs.at(id)->state[file].insert("refShift", ui->refEnergyShiftSpinBox->value());
                pairs.at(id)->state[file].insert("refCurv", ui->refCurvSpinBox->value());
            }
        }
        setOffset();
    }, Qt::QueuedConnection);


    //ui to this
    QObject::connect(ui->exportSummaryButton, &QPushButton::pressed, this, [=]{
        log(QString(this->metaObject()->className()) + "<- exportSummaryButton::pressed");
        setOffset(true);
        QFile outFile(ui->summaryPathLineEdit->text() + "/summary.csv");
        outFile.open(QIODevice::WriteOnly);
        QTextStream outStream(&outFile);
        for(int i = 0; i < summaryChart.series().size(); ++i){
            outStream << summaryChart.series().at(i)->name() + "_X" << "," << summaryChart.series().at(i)->name() + "_Y";
            if(i != summaryChart.series().size() - 1){
                outStream << ",";
            }
        }
        int row = 0;
        bool done = false;
        while(!done){
            done = true;
            outStream << endl;
            for(int i = 0; i < summaryChart.series().size(); ++i){
                QList<QPointF> points = dynamic_cast<QtCharts::QLineSeries *>(summaryChart.series().at(i))->points();
                if(i != 0){
                    outStream << ",";
                }
                if(row < points.size()){
                    done = false;
                    outStream <<  points.at(row).x() << "," << points.at(row).y();
                }else{
                    outStream <<  ",";
                }
            }
            row++;
        }
        outFile.close();
        setOffset();
    });
    QObject::connect(ui->fileTable, &QTableView::doubleClicked, this, [=](QModelIndex index){
        log(QString(this->metaObject()->className()) + "<- fileTable::doubleClicked");
        setOffset(true);
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
        setOffset();
    });
    QObject::connect(ui->file1Button, &QRadioButton::toggled, this, [=]{
        log(QString(this->metaObject()->className()) + "<- file1Button::toggled");
        setOffset(true);
        open(dataDir);
        setOffset();
    });
    QObject::connect(ui->tSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        log(QString(this->metaObject()->className()) + "<- tSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setLimits(ui->bSpinBox->value(), val, file, id);
        setOffset();
    });
    QObject::connect(ui->bSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        log(QString(this->metaObject()->className()) + "<- bSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setLimits(val, ui->tSpinBox->value(), file, id);
        setOffset();
    });
    QObject::connect(ui->rawBox, &QRadioButton::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- rawBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        raw[0].setVisible(state);
        raw[1].setVisible(state);
        if(state){
            while(!buttons.isEmpty()){
                buttons.takeLast()->setChecked(false);
            }
            buttons.append(ui->rawBox);
            rescale();
        }
        setOffset();
    });
    QObject::connect(ui->zeroBox, &QRadioButton::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- zeroBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        zero[0].setVisible(state);
        zero[1].setVisible(state);
        if(state){
            while(!buttons.isEmpty()){
                buttons.takeLast()->setChecked(false);
            }
            buttons.append(ui->zeroBox);
            rescale();
        }
        setOffset();
    });
    QObject::connect(ui->normBox, &QRadioButton::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- normBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        norm[0].setVisible(state && id >= 0);
        norm[1].setVisible(state && id >= 0);
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
        setOffset();
    });
    QObject::connect(ui->shadowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- shadowSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setShadowCurrent(val, ui->zeroShadowSpinBox->value(), file, id);
        setOffset();
    });
    QObject::connect(ui->zeroShadowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- zeroShadowSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setShadowCurrent(ui->shadowSpinBox->value(), val, file, id);
        setOffset();
    });
    QObject::connect(ui->smoothSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        log(QString(this->metaObject()->className()) + "<- smoothSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setSmooth(val, file, id);
        setOffset();
    });
    QObject::connect(ui->mulBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- mulBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        ui->mulCoeffSpinBox->setEnabled(state);
        ui->linearBox->setEnabled(state);
        ui->particularCurvatureSpinBox->setEnabled(state);
        setNormalizationCoeff(ui->mulCoeffSpinBox->value(), state, file, id, 0);
        setNormalizationCoeff(0.0, state, file, id, 0);
        if(state){
            buttons.append(ui->mulBox);
            setRelativeCurv(ui->particularCurvatureSpinBox->value(), file, id);
        }else{
            buttons.takeLast()->setChecked(false);
        }
        setOffset();
    });
    QObject::connect(ui->mulCoeffSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- mulCoeffSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setNormalizationCoeff(val, ui->mulBox->isChecked(), file, id, 0);
        setOffset();
    });
    QObject::connect(ui->particularCurvatureSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- particularCurvatureSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setRelativeCurv(val, file, id);
        setOffset();
    });
    QObject::connect(ui->linearBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- linearBox::toggled. state = "  + QString::number(state));
        setOffset(true);
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
        setOffset();
    });
    QObject::connect(ui->llSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- llSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setLinearIntervals(QPointF(val, ui->rlSpinBox->value()), ui->linearBox->isChecked(), file, id);
        setOffset();
    });
    QObject::connect(ui->rlSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- rlSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setLinearIntervals(QPointF(ui->llSpinBox->value(), val), ui->linearBox->isChecked(), file, id);
        setOffset();
    });
    QObject::connect(ui->linearBackgroundBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- linearBackgroundBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        setLin(state, file, id);
        ui->filletSpinBox->setEnabled(state);
        ui->steppedBackgroundBox->setEnabled(state);
        line[0].setVisible(!state);
        line[1].setVisible(!state);
        steps.setVisible(state);
        if(state){
            buttons.append(ui->linearBackgroundBox);
        }else{
            buttons.takeLast()->setChecked(false);
        }
        rescale();
        setOffset();
    });
    QObject::connect(ui->filletSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- filletSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setStepped(val, ui->steppedBackgroundBox->isChecked(), file, id);
        setOffset();
    });
    QObject::connect(ui->steppedBackgroundBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- steppedBackgroundBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        setStepped(ui->filletSpinBox->value(), state, file, id);
        ui->refCheckBox->setEnabled(state);
        ui->diffBox->setEnabled(state);
        steps.setVisible(!state);
        if(state){
            buttons.append(ui->steppedBackgroundBox);
        }else{
            ui->refCheckBox->setChecked(false);
            buttons.takeLast()->setChecked(false);
        }
        setOffset();
    });
    QObject::connect(ui->refCheckBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- refCheckBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        ui->refEnergyShiftSpinBox->setEnabled(state);
        ui->refCurvSpinBox->setEnabled(state);
        ui->useRefCheckBox->setEnabled(state && (ui->integrateBox->isChecked()));
        activateRef(state, id, file);
        refData[0].setVisible(state);
        refData[1].setVisible(state);
        refxmcd.setVisible(ui->diffBox->isChecked() && state);
        if(state){
            setNormalizationCoeff(ui->refCurvSpinBox->value(), true, file, id, 1);
        }else{
            ui->useRefCheckBox->setChecked(false);
        }
        setOffset();
    });
    QObject::connect(ui->refEnergyShiftSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- refEnergyShiftSpinBox::toggled. val = "  + QString::number(val));
        setOffset(true);
        setEnergyShift(val, file, id);
        setOffset();
    });
    QObject::connect(ui->refCurvSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- refCurvSpinBox::toggled. val = "  + QString::number(val));
        setOffset(true);
        setNormalizationCoeff(val, true, file, id, 1);
        setOffset();
    });
    QObject::connect(ui->diffBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- diffBox::toggled. state. = "  + QString::number(state));
        setOffset(true);
        ui->integrationLimitSpinBox->setValue(raw[file].pointsVector().size()/2);
        xmcd.setVisible(state);
        refxmcd.setVisible(ui->refCheckBox->isChecked() && state);
        xmcdZero.setVisible(state);
        axisY2.setVisible(state);
        ui->integrateBox->setEnabled(state);
        if(state){
            buttons.append(ui->diffBox);
        }else{
            buttons.takeLast()->setChecked(false);
        }
        setOffset();
    });
    QObject::connect(ui->integrateBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- integrateBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        ui->integrationLimitSpinBox->setEnabled(state);
        ui->positiveCheckBox->setEnabled(state);
        ui->groundCheckBox->setEnabled(state);
        ui->useRefCheckBox->setEnabled(state && ui->refCheckBox->isChecked());
        dot.setVisible(state);
        setIntegrate(state, ui->integrationLimitSpinBox->value(), file, id, 0);
        integrated[file] = state;
        ui->calculateBox->setEnabled(state && (integrated[0] && integrated[1]));
        if(state){
            dot.replace(0, xmcd.at(ui->integrationLimitSpinBox->value()));
            buttons.append(ui->integrateBox);
        }else{
            ui->useRefCheckBox->setChecked(false);
            summLabels[file]->setText("");
            dl2Labels[file]->setText("");
            dl3Labels[file]->setText("");
            mSELabels[file]->setText("");
            mOLabels[file]->setText("");
            relationLabels[file]->setText("");
            buttons.takeLast()->setChecked(false);
        }
        setOffset();
    });
    QObject::connect(ui->integrationLimitSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int val){
        log(QString(this->metaObject()->className()) + "<- integrationLimitSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        dot.replace(0, xmcd.at(ui->integrationLimitSpinBox->value()));
        setIntegrate(ui->integrateBox->isChecked(), val, file, id, 0);
        if(ui->refCheckBox->isChecked()){
            setIntegrate(ui->integrateBox->isChecked(), dataPointers[file].at(id)->at(val).first, file, id, 1);
        }
        setOffset();
    });
    QObject::connect(ui->positiveCheckBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- positiveBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        setPositiveIntegrals(state, file, id);
        setOffset();
    });
    QObject::connect(ui->groundCheckBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- groundBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        setGround(state, file, id);
        setOffset();
    });
    QObject::connect(ui->useRefCheckBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- useRefBox::toggled. state = "  + QString::number(state));
        setOffset(true);
        emit setArea(state, pairs.at(id)->state[file].value("refSummArea", 1.0).toDouble(), file, id);
        setOffset();
    });
    QObject::connect(ui->calculateBox, &QCheckBox::toggled, this, [=](bool state){
        log(QString(this->metaObject()->className()) + "<- calculateBox::toggled. state = "  + QString::number(state));
        setOffset(true);
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
        setOffset();
    });
    QObject::connect(ui->phi1OffsetSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- phi1OffsetSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        qreal phi1 = qDegreesToRadians(theta[0] + ui->phi1OffsetSpinBox->value());
        qreal phi2 = qDegreesToRadians(theta[1] + ui->phi2OffsetSpinBox->value());
        bool state = ui->calculateBox->isChecked() && integrated[0] && integrated[1];
        setCalculate(state, QPointF(phi1, phi2), QPointF(qDegreesToRadians(theta[0]), qDegreesToRadians(theta[1])), id);
        phiLabels[0]->setText(QString::number(theta[0] + offsets[0]->value()));
        setOffset();
    });
    QObject::connect(ui->phi2OffsetSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- phi2OffsetSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        qreal phi1 = qDegreesToRadians(theta[0] + ui->phi1OffsetSpinBox->value());
        qreal phi2 = qDegreesToRadians(theta[1] + ui->phi2OffsetSpinBox->value());
        bool state = ui->calculateBox->isChecked() && integrated[0] && integrated[1];
        setCalculate(state, QPointF(phi1, phi2), QPointF(qDegreesToRadians(theta[0]), qDegreesToRadians(theta[1])), id);
        phiLabels[1]->setText(QString::number(theta[1] + offsets[1]->value()));
        setOffset();
    });
    QObject::connect(ui->pSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- pSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setIntegrationConstants(val, ui->nSpinBox->value(), id);
        setOffset();
    });
    QObject::connect(ui->nSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](double val){
        log(QString(this->metaObject()->className()) + "<- nSpinBox::valueChanged. val = "  + QString::number(val));
        setOffset(true);
        setIntegrationConstants(ui->pSpinBox->value(), val, id);
        setOffset();
    });
    QObject::connect(ui->exportButton, &QPushButton::pressed, this, [=](){
        log(QString(this->metaObject()->className()) + "<- exportBox::pressed");
        setOffset(true);
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
        setOffset();
    });
    QObject::connect(ui->swapButton, &QPushButton::pressed, this, [=](){
        log(QString(this->metaObject()->className()) + "<- swapBox::pressed");
        setOffset(true);
        if(file == 0){
            pairs[id]->ui.file2Button->setChecked(true);
        }else{
            pairs[id]->ui.file1Button->setChecked(true);
        }
        setOffset();
    });
    QObject::connect(ui->setExportPathButton, &QPushButton::pressed, this, &MainWindow::exportDialog);
    QObject::connect(ui->setExportPath2Button, &QPushButton::pressed, this, &MainWindow::exportDialog);
    QObject::connect(ui->selectPathButton, &QPushButton::pressed, this, [=](){
        log(QString(this->metaObject()->className()) + "<- selectPathButton::pressed");
        setOffset(true);
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select data directory"), dataDir);
        if(dir.length() != 0){
            refresh->removePath(dataDir);
            dataDir = dir;
            open(dir);
            refresh->addPath(dir);
        }
        setOffset();
    });
    QObject::connect(refresh, &QFileSystemWatcher::directoryChanged, this, &MainWindow::open);
    QObject::connect(ui->pairButton, &QPushButton::pressed, this, [=](){
        log(QString(this->metaObject()->className()) + "<- pairButton::pressed");
        setOffset(true);
        if(fileName[0].size() != 0 && fileName[1].size() != 0){
            pairs.append(new PairWidget());
            wrapper->appendCalc();
            for(int i = 0; i < 2; ++i){
                dataPointers[i].append(nullptr);
                rawPointers[i].append(nullptr);
                zeroPointers[i].append(nullptr);
                stepPointers[i].append(nullptr);
                xmcdPointers[i].append(nullptr);
                refDataPointers[i].append(nullptr);
                refXmcdPointers[i].append(nullptr);
            }
            int id = pairs.count() - 1;
            ui->pairTable->insertRow(id);
            ui->pairTable->setRowHeight(id, pairs.last()->height());
            ui->pairTable->setCellWidget(id, 0, pairs.last());
            for(int file = 0; file < 2 ; ++file){
                fileNameLabel[file]->setText("");
                pairs.last()->state[file] = defaults();
                pairs.last()->fileLabels[file]->setText(fileName[file]);
                log(filePath[file] + "/" + fileName[file]);
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
                log(QString(this->metaObject()->className()) + "<- pair::selected");
                setOffset(true);
                selectPair(pairs.indexOf(dynamic_cast<PairWidget *>(sender())));
                setOffset();
            });
            QObject::connect(pairs.last(), &PairWidget::deletePressed, this, [=]{
                log(QString(this->metaObject()->className()) + "<- pair::deletePressed. id = " + QString::number(id));
                setOffset(true);
                int id = pairs.indexOf(dynamic_cast<PairWidget *>(sender()));
                PairWidget *curr = pairs.at(id);
                ui->pairTable->removeRow(id);
                pairs.removeAt(id);
                ui->summaryTable->removeRow(2*id + 1);
                ui->summaryTable->removeRow(2*id);
                for(int i = 0; i < 2; ++i){
                    dataPointers[i].removeAt(id);
                    rawPointers[i].removeAt(id);
                    zeroPointers[i].removeAt(id);
                    stepPointers[i].removeAt(id);
                    xmcdPointers[i].removeAt(id);
                    refDataPointers[i].removeAt(id);
                    refXmcdPointers[i].removeAt(id);
                }
                wrapper->removeCalc(id);
                if(id == this->id){
                    file = 0;
                    if(pairs.size() > 0){
                        if(id == pairs.size()){
                            id --;
                        }
                        selectPair(id);
                    }else{
                        this->id = -1;
                        loadState(defaults());
                    }
                }
                updateSummary();
                delete curr;
                setOffset();
            });
            QObject::connect(pairs.last(), &PairWidget::fileSelected, this, [=](int file){
                log(QString(this->metaObject()->className()) + "<- pair::fileSelected. file = " + QString::number(file));
                setOffset(true);
                this->file = file;
                loadState(pairs.at(id)->state[file]);
                chart.setTitle(pairs.at(id)->state[file].value("sample", "null").toString() + " " +
                        pairs.at(id)->state[file].value("energy", "null").toString() + " " +
                        QString::number(pairs.at(id)->state[file].value("theta", -1.0).toDouble()));
                if(file == 0){
                    ui->file1GroupBox->setStyleSheet("background-color: rgb(200, 200, 0);");
                    ui->file2GroupBox->setStyleSheet("background-color: rgb(212, 208, 200);");
                }else{
                    ui->file2GroupBox->setStyleSheet("background-color: rgb(200, 200, 0);");
                    ui->file1GroupBox->setStyleSheet("background-color: rgb(212, 208, 200);");
                }
                update(file, id);
                setOffset();
            });
            integrated[0] = false;
            integrated[1] = false;
            int did = 2*id;
            ui->summaryTable->insertRow(did);
            ui->summaryTable->insertRow(did + 1);
            QCheckBox* use = new QCheckBox(pairs.last());
            QDoubleSpinBox* x1Offset = new QDoubleSpinBox(pairs.last());
            x1Offset->setDecimals(1);
            QDoubleSpinBox* x2Offset = new QDoubleSpinBox(pairs.last());
            x2Offset->setDecimals(1);
            QDoubleSpinBox* yOffset = new QDoubleSpinBox(pairs.last());
            yOffset->setDecimals(1);
            QSpinBox* yOffsetMult = new QSpinBox(pairs.last());
            use->connect(use, &QCheckBox::toggled, this, [=](bool state){
               x1Offset->setEnabled(state);
               x2Offset->setEnabled(state);
               yOffset->setEnabled(state);
               yOffsetMult->setEnabled(state);
               updateSummary();
            });
            ui->summaryTable->setCellWidget(did, 0, use);
            use->setToolTip(summaryTooltips.at(0));
            ui->summaryTable->setSpan(did, 0, 2, 1);
            ui->summaryTable->setItem(did, 1, new QTableWidgetItem(sample + " " + energy + " " +
                                                                  QString::number(pairs.last()->state[0].value("theta",
                                                                                                               -1.0).toDouble())));
            ui->summaryTable->item(did, 1)->setToolTip(summaryTooltips.at(1));
            ui->summaryTable->setItem(did + 1, 1, new QTableWidgetItem(sample + " " + energy + " " +
                                                                  QString::number(pairs.last()->state[1].value("theta",
                                                                                                               -1.0).toDouble())));
            ui->summaryTable->item(did + 1, 1)->setToolTip(summaryTooltips.at(1));
            ui->summaryTable->setItem(did, 2, new QTableWidgetItem("signal"));
            ui->summaryTable->item(did, 2)->setToolTip(summaryTooltips.at(2));
            ui->summaryTable->setItem(did + 1, 2, new QTableWidgetItem("signal"));
            ui->summaryTable->item(did + 1, 2)->setToolTip(summaryTooltips.at(2));
            ui->summaryTable->setItem(did, 3, new QTableWidgetItem("mS/mO"));
            ui->summaryTable->item(did, 3)->setToolTip(summaryTooltips.at(3));
            ui->summaryTable->setItem(did + 1, 3, new QTableWidgetItem("mS/mO"));
            ui->summaryTable->item(did + 1, 3)->setToolTip(summaryTooltips.at(3));
            ui->summaryTable->setItem(did, 4, new QTableWidgetItem("mS"));
            ui->summaryTable->setSpan(did, 4, 2, 1);
            ui->summaryTable->item(did, 4)->setToolTip(summaryTooltips.at(4));
            ui->summaryTable->setItem(did, 5, new QTableWidgetItem("mT"));
            ui->summaryTable->item(did, 5)->setToolTip(summaryTooltips.at(5));
            ui->summaryTable->setSpan(did, 5, 2, 1);
            ui->summaryTable->setItem(did, 6, new QTableWidgetItem("mOrb∥"));
            ui->summaryTable->item(did, 6)->setToolTip(summaryTooltips.at(6));
            ui->summaryTable->setSpan(did, 6, 2, 1);
            ui->summaryTable->setItem(did, 7, new QTableWidgetItem("mOrb⊥"));
            ui->summaryTable->item(did, 7)->setToolTip(summaryTooltips.at(7));
            ui->summaryTable->setSpan(did, 7, 2, 1);
            ui->summaryTable->setItem(did, 8, new QTableWidgetItem("mOrb"));
            ui->summaryTable->item(did, 8)->setToolTip(summaryTooltips.at(8));
            ui->summaryTable->setSpan(did, 8, 2, 1);
            x1Offset->setMinimum(-200.0);
            x1Offset->setMaximum(200.0);
            x1Offset->setSingleStep(0.1);
            x1Offset->connect(x1Offset, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                              this, [=](double val){
                log(QString(this->metaObject()->className()) + "<- x1Offset::valueChanged. val = "  + QString::number(val));
                setOffset(true);
                updateSummary();
                setOffset();
            });
            ui->summaryTable->setCellWidget(did, 9, x1Offset);
            x1Offset->setToolTip(summaryTooltips.at(9));
            x2Offset->setMinimum(-200.0);
            x2Offset->setMaximum(200.0);
            x2Offset->setSingleStep(0.1);
            x2Offset->connect(x2Offset, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                              this, [=](double val){
                log(QString(this->metaObject()->className()) + "<- x2Offset::valueChanged. val = "  + QString::number(val));
                setOffset(true);
                updateSummary();
                setOffset();
            });
            ui->summaryTable->setCellWidget(did + 1, 9, x2Offset);
            x2Offset->setToolTip(summaryTooltips.at(9));
            yOffset->setMinimum(-15.0);
            yOffset->setMaximum(15.0);
            yOffset->setSingleStep(0.1);
            yOffset->connect(yOffset, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             this, [=](double val){
                log(QString(this->metaObject()->className()) + "<- yOffset::valueChanged. val = "  + QString::number(val));
                setOffset(true);
                updateSummary();
                setOffset();
            });
            ui->summaryTable->setCellWidget(did, 10, yOffset);
            yOffset->setToolTip(summaryTooltips.at(10));
            ui->summaryTable->setSpan(did, 10, 2, 1);
            yOffsetMult->setMinimum(-12);
            yOffsetMult->setMaximum(10);
            yOffsetMult->setSingleStep(1);
            yOffsetMult->setValue(offsetMult);
            yOffsetMult->setPrefix("10^");
            yOffsetMult->connect(yOffsetMult, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int val){
                log(QString(this->metaObject()->className()) + "<- yOffsetMult::valueChanged. val = "  + QString::number(val));
                setOffset(true);
                updateSummary();
                setOffset();
            });
            ui->summaryTable->setCellWidget(did, 11, yOffsetMult);
            yOffsetMult->setToolTip(summaryTooltips.at(11));
            ui->summaryTable->setSpan(did, 11, 2, 1);
            selectPair(id);
            emit ui->swapButton->pressed();
        }
        setOffset();
    });
    QObject::connect(ui->summaryButtonGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), this, [=](int buttonId, bool state){
        log(QString(this->metaObject()->className()) + "<- summaryButtonGroup::toggled. buttonId = " + QString::number(buttonId) +
            ". state = " + state);
        setOffset(true);
        if(state){
            updateSummary();
        }
        setOffset();
    });
    QObject::connect(ui->summaryNormSlider, &QSlider::valueChanged, this, [=](int val){
        log(QString(this->metaObject()->className()) + "<- summaryNormSlider::valueChanged. val = " + QString::number(val));
        setOffset(true);
        if(ui->summaryButtonGroup->checkedId() >= 2){
            updateSummary();
        }
        setOffset();
    });
    loadSettings();
    ui->pairTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->pairTable->setColumnCount(1);
    ui->pairTable->horizontalHeader()->setHidden(true);
    ui->pairTable->setColumnWidth(0, 220);
    calcThread->start();
    if(!normalExit){
        log("crash detected");
    }
    loadState(defaults());
    ui->unitBox->setEnabled(false); //disabled in current version
}

MainWindow::~MainWindow()
{
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    delete refresh;
    calcThread->quit();
    calcThread->requestInterruption();
    if(calcThread->wait()){
    }else{
        log("Error: thread exit error");
    }
    delete calcThread;
    saveSettings();
    delete table;
    delete ui;
}

void MainWindow::exportDialog(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select export directory"), dataDir);
    if(dir.length() != 0){
        ui->exportLine->setText(dir);
        ui->summaryPathLineEdit->setText(dir);
        ui->exportSummaryButton->setEnabled(true);
    }
    setOffset();
}

void MainWindow::selectPair(const int id){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + ". " + QString::number(id));
    setOffset(true);
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
    setOffset();
}

void MainWindow::rescale(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
    QPointF x(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QPointF y(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    QList<QtCharts::QAbstractSeries*> series = chart.series();
    for(int i = 0; i < series.size(); ++i){
        QtCharts::QAbstractSeries* ser = series.at(i);
        if(ser && ser != &dot && ser != &xmcd && ser != &xmcdZero){
            if(ser->isVisible()){
                QtCharts::QLineSeries * lineSer = dynamic_cast<QtCharts::QLineSeries *>(ser);
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

    }
    qreal scale = y.y() - y.x();
    y.rx() = y.x() - scale * 0.02;
    y.ry() = y.y() + scale * 0.02;
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
    setOffset();
}

void MainWindow::loadSettings(){
    QSettings settings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    setOffset(true);
    normalExit = settings.value("normalExit", false).toBool();
    version = settings.value("version", "v???").toString();
    settings.setValue("normalExit", false);
    settings.beginGroup("gui");
        resize(settings.value("windowSize", QSize(800, 600)).toSize());
        move(settings.value("windowPosition", QPoint(0, 0)).toPoint());
        windowTitle = settings.value("windowTitle", "XMCDAnalyzer(corrupted .ini)").toString();
        setWindowTitle(windowTitle + " " + version);
        dataDir = settings.value("lastDir", "").toString();
        int lineCount = settings.beginReadArray("lines");
        for(int i = 0; i < lineCount; ++i){
            settings.setArrayIndex(i);
            QString name = settings.value("name", "Unknown").toString();
            int r = settings.value("r", 0).toInt();
            int g = settings.value("g", 0).toInt();
            int b = settings.value("b", 0).toInt();
            lineColours.insert(name, QColor(r, g, b));
        }
        settings.endArray();
        raw[0].setColor(lineColours.value("raw0", QColor(0, 0, 0)));
        raw[1].setColor(lineColours.value("raw1", QColor(0, 0, 0)));
        norm[0].setColor(lineColours.value("norm0", QColor(0, 0, 0)));
        norm[1].setColor(lineColours.value("norm1", QColor(0, 0, 0)));
        refData[0].setColor(lineColours.value("refData0", QColor(0, 0, 0)));
        refData[1].setColor(lineColours.value("refData1", QColor(0, 0, 0)));
        zero[0].setColor(lineColours.value("zero0", QColor(0, 0, 0)));
        zero[1].setColor(lineColours.value("zero1", QColor(0, 0, 0)));
        xmcd.setColor(lineColours.value("xmcd", QColor(0, 0, 0)));
        refxmcd.setColor(lineColours.value("refXmcd", QColor(0, 0, 0)));
        xmcdZero.setColor(lineColours.value("xmcdZero", QColor(0, 0, 0)));
        dot.setColor(lineColours.value("dot", QColor(0, 0, 0)));
        steps.setColor(lineColours.value("steps", QColor(0, 0, 0)));
        line[0].setColor(lineColours.value("line0", QColor(0, 0, 0)));
        line[1].setColor(lineColours.value("line1", QColor(0, 0, 0)));
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
                summaryTooltips.append(settings.value("tooltip", "Tooltip not found. Settings file may be corrupted.").toString());
                ui->summaryTable->horizontalHeaderItem(i)->setToolTip(summaryTooltips.at(i));
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
    settings.beginGroup("defaultValues");
        ui->nSpinBox->setValue(settings.value("nh", 2.49).toDouble());
        ui->pSpinBox->setValue(settings.value("pc", 0.7).toDouble());
        offsetMult = settings.value("normOffsetMult", -6).toInt();
        stepSize = settings.value("stepSize", 10.0).toDouble();
        refCommonPath = settings.value("refPath", QApplication::applicationDirPath() + "/res/ref/").toString();
        wrapper->setRefPaths(refCommonPath);
    settings.endGroup();
    settings.beginGroup("logging");
        logEnabled = settings.value("enabled", false).toBool();
        logPath = settings.value("path", "").toString();
        useExeLocation = settings.value("useExeLocation", true).toBool();
        debugMode = settings.value("debug", false).toBool();
        debugCopy = settings.value("copy", false).toBool();
        if(useExeLocation){
            logPath = QApplication::applicationDirPath();
        }
        if(logEnabled){
            if(debugMode){
                logger = new Logger("", debugCopy, this);
            }else{
                if(!normalExit && QFile(logPath + "/log.txt").exists()){
                    QString debugLogs = logPath + "/crashLogs/";
                    if(!QDir(debugLogs).exists()){
                        QDir().mkdir(debugLogs);
                    }
                    QFile old(logPath + "/log.txt");
                    old.copy(debugLogs + old.fileTime(QFileDevice::FileModificationTime).toString("dMMMMhhmmss") + ".txt");
                }
                logger = new Logger(logPath, debugCopy, this);
            }
            QObject::connect(this, &MainWindow::log, logger, &Logger::print);
            QObject::connect(this, &MainWindow::setOffset, logger, &Logger::addOffset);
        }
    settings.endGroup();
    setOffset();
}

void MainWindow::saveSettings(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
    QSettings settings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.clear();
    settings.setValue("normalExit", true);
    settings.setValue("version", version);
    settings.beginGroup("gui");
        settings.setValue("windowSize", this->size());
        settings.setValue("windowPosition", this->pos());
        settings.setValue("windowTitle", windowTitle);
        settings.setValue("lastDir", dataDir);
        settings.beginWriteArray("lines");
            int i = 0;
            settings.setArrayIndex(i);
            settings.setValue("name", "raw0");
            settings.setValue("r", raw[0].color().red());
            settings.setValue("g", raw[0].color().green());
            settings.setValue("b", raw[0].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "raw1");
            settings.setValue("r", raw[1].color().red());
            settings.setValue("g", raw[1].color().green());
            settings.setValue("b", raw[1].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "norm0");
            settings.setValue("r", norm[0].color().red());
            settings.setValue("g", norm[0].color().green());
            settings.setValue("b", norm[0].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "norm1");
            settings.setValue("r", norm[1].color().red());
            settings.setValue("g", norm[1].color().green());
            settings.setValue("b", norm[1].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "refData0");
            settings.setValue("r", refData[0].color().red());
            settings.setValue("g", refData[0].color().green());
            settings.setValue("b", refData[0].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "refData1");
            settings.setValue("r", refData[1].color().red());
            settings.setValue("g", refData[1].color().green());
            settings.setValue("b", refData[1].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "zero0");
            settings.setValue("r", zero[0].color().red());
            settings.setValue("g", zero[0].color().green());
            settings.setValue("b", zero[0].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "zero1");
            settings.setValue("r", zero[1].color().red());
            settings.setValue("g", zero[1].color().green());
            settings.setValue("b", zero[1].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "xmcd");
            settings.setValue("r", xmcd.color().red());
            settings.setValue("g", xmcd.color().green());
            settings.setValue("b", xmcd.color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "refXmcd");
            settings.setValue("r", refxmcd.color().red());
            settings.setValue("g", refxmcd.color().green());
            settings.setValue("b", refxmcd.color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "xmcdZero");
            settings.setValue("r", xmcdZero.color().red());
            settings.setValue("g", xmcdZero.color().green());
            settings.setValue("b", xmcdZero.color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "dot");
            settings.setValue("r", dot.color().red());
            settings.setValue("g", dot.color().green());
            settings.setValue("b", dot.color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "steps");
            settings.setValue("r", steps.color().red());
            settings.setValue("g", steps.color().green());
            settings.setValue("b", steps.color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "line0");
            settings.setValue("r", line[0].color().red());
            settings.setValue("g", line[0].color().green());
            settings.setValue("b", line[0].color().blue());
            ++i;
            settings.setArrayIndex(i);
            settings.setValue("name", "line1");
            settings.setValue("r", line[1].color().red());
            settings.setValue("g", line[1].color().green());
            settings.setValue("b", line[1].color().blue());
        settings.endArray();
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
                    settings.setValue("tooltip", summaryTooltips.at(i));
                }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
    settings.beginGroup("defaultValues");
        settings.setValue("nh", ui->nSpinBox->value());
        settings.setValue("pc", ui->pSpinBox->value());
        settings.setValue("normOffsetMult", offsetMult);
        settings.setValue("stepSize", stepSize);
        settings.setValue("refPath", refCommonPath);
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
    setOffset();
}

void MainWindow::open(QString selectedPath){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__ + " " + selectedPath);
    setOffset(true);
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
    setOffset();
}

void MainWindow::loadState(const QHash<QString, QVariant> state){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
    bool loaded = state.value("loaded", false).toBool();
    if(loaded){
        pairs.at(id)->fileButtons[file]->setChecked(true);
        setLoader(state.value("filename", "").toString(), file, id, state.value("energy", "").toString());
        setIntegrationConstants(ui->pSpinBox->value(), ui->nSpinBox->value(), id);
        theta[file] = state.value("theta", 0.0).toDouble();
        thetaLabels[file]->setText(QString::number(theta[file]));
        offsets[file]->setEnabled(true);
        ui->swapButton->setEnabled(true);
        ui->rawBox->setEnabled(true);
        ui->zeroBox->setEnabled(true);
        ui->normBox->setEnabled(true);
    }else{
        for(int i = 0; i < 2 ; ++i){
            zero[i].setVisible(false);
            raw[i].setVisible(false);
            norm[i].setVisible(false);
            refData[i].setVisible(false);
            line[i].setVisible(false);
        }
        steps.setVisible(false);
        xmcd.setVisible(false);
        refxmcd.setVisible(false);
        dot.setVisible(false);
        chart.setTitle("");
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
    ui->refCheckBox->setChecked(state.value("ref", false).toBool());
    ui->refEnergyShiftSpinBox->setValue(state.value("refShift", 0.0).toDouble());
    ui->refCurvSpinBox->setValue(state.value("refCurv", 0.0).toDouble());
    ui->diffBox->setChecked(state.value("xmcd", false).toBool());
    ui->integrateBox->setChecked(state.value("integrate", false).toBool());
    ui->integrationLimitSpinBox->setValue(state.value("integrationLimit", 0).toInt());
    ui->positiveCheckBox->setChecked(state.value("positiveIntegrals", false).toBool());
    ui->groundCheckBox->setChecked(state.value("ground", false).toBool());
    if(loaded){
        offsets[file]->setValue(state.value("phiOff", 0.0).toDouble());
    }else{
        for(int i = 0; i < 2; ++i){
            offsets[i]->setValue(0);
        }
    }
    if(loaded){
        phiLabels[file]->setText(QString::number(theta[file] + offsets[file]->value()));
    }
    ui->calculateBox->setChecked(state.value("calc", false).toBool());
    rescale();
    log("state loaded");
    setOffset();
}


QHash<QString, QVariant> MainWindow::defaults(){
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
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
    tmp.insert("ref", false);
    tmp.insert("refShift", 0.0);
    tmp.insert("refCurv", 0.0);
    tmp.insert("xmcd", false);
    tmp.insert("integrationLimit", 0);
    tmp.insert("integrate", false);
    tmp.insert("positiveIntegrals", false);
    tmp.insert("ground", false);
    tmp.insert("calc", false);
    tmp.insert("phiOff", 0.0);
    tmp.insert("theta", 0.0);
    tmp.insert("sample", "null");
    tmp.insert("energy", "null");
    setOffset();
    return tmp;
}

void MainWindow::updateSummary(){ //make it faster, parsing id of the changed row
    log(QString(this->metaObject()->className()) + "::" + __FUNCTION__);
    setOffset(true);
    summaryChart.removeAllSeries();
    qreal xMin = std::numeric_limits<double>::max();
    qreal yMin = std::numeric_limits<double>::max();
    qreal xMax = std::numeric_limits<double>::min();
    qreal yMax = std::numeric_limits<double>::min();
    bool badNormalization = false;
    for(int i = 0; i < (int)(ui->summaryTable->rowCount()/2); ++i){
        int doubleI = 2*i;
        if(dynamic_cast<QCheckBox *>(ui->summaryTable->cellWidget(doubleI, 0))->isChecked()){
            for(int j = 0; j < 2; ++j){
                qreal xOff = dynamic_cast<QDoubleSpinBox *>(ui->summaryTable->cellWidget(doubleI + j, 9))->value();
                qreal yOff = dynamic_cast<QDoubleSpinBox *>(ui->summaryTable->cellWidget(doubleI, 10))->value() *
                        qPow(10, dynamic_cast<QSpinBox *>(ui->summaryTable->cellWidget(doubleI, 11))->value());
                qreal mult = 1.0;
                qreal maxRelation = 0.0;
                ui->summaryTable->item(doubleI + j, 2)->setBackgroundColor(Qt::white);
                if(ui->summaryNormSlider->value() == 1){
                    if(stepPointers[j].at(i) != nullptr){
                        mult = stepSize/(stepPointers[j].at(i)->last().y() - stepPointers[j].at(i)->first().y());
                    }else{
                        badNormalization = true;
                        ui->summaryTable->item(doubleI + j, 2)->setBackgroundColor(Qt::red);
                    }
                }else{
                    if(dataPointers[j].at(i) != nullptr){
                        qreal max = std::numeric_limits<double>::min();
                        for(int k = 0; k < dataPointers[j].at(i)->size(); ++k){
                            qreal halfSum = (dataPointers[j].at(i)->at(k).second.x() + dataPointers[j].at(i)->at(k).second.y())/2;
                            if(halfSum > max){
                                max = halfSum;
                                maxRelation = qAbs(dataPointers[j].at(i)->at(k).second.x() - dataPointers[j].at(i)->at(k).second.y())/max;
                            }
                        }
                        mult = stepSize/(max - (dataPointers[j].at(i)->first().second.x() + dataPointers[j].at(i)->first().second.y())/2);
                        if(ui->summaryButtonGroup->checkedId() >= 2){
                            const QSignalBlocker preventUpdate(ui->summaryTable->cellWidget(doubleI, 11));
                            dynamic_cast<QSpinBox *>(ui->summaryTable->cellWidget(doubleI, 11))->setValue(qFloor(qLn(max*mult)/M_LN10)
                                                                                                          - 1);
                        }
                    }
                }
                ui->summaryTable->item(doubleI + j, 2)->setText(QString::number(maxRelation));
                if(ui->summaryButtonGroup->checkedId() < 3 && ui->summaryButtonGroup->checkedId() >= 0){
                    QVector<QPair<qreal, QPointF>> const * data;
                    bool ok = true;
                    switch (ui->summaryButtonGroup->checkedId()) {
                    case 0:
                        data = rawPointers[j].at(i);
                        break;
                    case 1:
                        data = zeroPointers[j].at(i);
                        break;
                    case 2:
                        data = dataPointers[j].at(i);
                        break;
                    default:
                        ok = false;
                        log(QString(this->metaObject()->className()) + "::" +  __FUNCTION__  + "unknown button id");
                        break;
                    }
                    if(ok && (data != nullptr)){
                        QtCharts::QLineSeries* line = new QtCharts::QLineSeries(ui->summaryChartWidget);
                        line->setName(ui->summaryTable->item(doubleI + j, 1)->text());
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
                        ui->summaryTable->item(doubleI + j, 1)->setBackgroundColor(line->color());
                    }else{
                        //calc thread still working
                    }
                }else if(ui->summaryButtonGroup->checkedId() == 3){
                    QVector<QPointF> const * data = xmcdPointers[j].at(i);
                    if(data != nullptr){
                        QtCharts::QLineSeries* line = new QtCharts::QLineSeries(ui->summaryChartWidget);
                        line->setName(ui->summaryTable->item(doubleI + j, 1)->text());
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
                        ui->summaryTable->item(doubleI + j, 1)->setBackgroundColor(line->color());
                    }else{
                        //calc thread still working
                    }
                }else{
                    QVector<QPair<qreal, QPointF>> const * data = dataPointers[j].at(i);
                    if(data != nullptr){
                        QtCharts::QLineSeries* line = new QtCharts::QLineSeries(ui->summaryChartWidget);
                        line->setName(ui->summaryTable->item(doubleI + j, 1)->text());
                        for(int k = 0; k < data->size(); ++k){
                            line->append(data->at(k).first + xOff, (data->at(k).second.x() + data->at(k).second.y())*mult/2.0 + yOff);
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
                        ui->summaryTable->item(doubleI + j, 1)->setBackgroundColor(line->color());
                    }else{
                        //calc thread still working
                    }
                }
            }
        }else{
            ui->summaryTable->item(doubleI, 1)->setBackgroundColor(Qt::white);
            ui->summaryTable->item(doubleI + 1, 1)->setBackgroundColor(Qt::white);
        }
    }
    if(badNormalization){
        ui->summaryNormButton->setStyleSheet("background-color: red;");
    }else{
        ui->summaryNormButton->setStyleSheet("background-color: rgb(212, 208, 200);");
    }
    log("reloaded curves");
    if(xMin != std::numeric_limits<double>::max()){
        if(xMin != summaryAxisX.min() && xMax != summaryAxisX.max()){
            summaryAxisX.setRange(xMin, xMax);
        }else if(xMin != summaryAxisX.min()){
            summaryAxisX.setMin(xMin);
        }else if(xMax != summaryAxisX.max()){
            summaryAxisX.setMax(xMax);
        }
        if(yMin != summaryAxisY.min() && yMax != summaryAxisY.max()){
            summaryAxisY.setRange(yMin, yMax);
        }else if(yMin != summaryAxisY.min()){
            summaryAxisY.setMin(yMin);
        }else if(yMax != summaryAxisY.max()){
            summaryAxisY.setMax(yMax);
        }
    }
    QtCharts::QLineSeries* line = new QtCharts::QLineSeries(ui->summaryChartWidget);
    line->setName("0");
    line->append(summaryAxisX.min(), 0.0);
    line->append(summaryAxisX.max(), 0.0);
    summaryChart.addSeries(line);
    line->setColor(Qt::black);
    line->attachAxis(&summaryAxisX);
    line->attachAxis(&summaryAxisY);
    log("rescaled");
    setOffset();
}
