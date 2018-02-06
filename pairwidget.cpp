#include <QtWidgets>

#include "pairwidget.h"

PairWidget::PairWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    fileLabels[0] = ui.file1Label;
    fileLabels[1] = ui.file2Label;
    fileButtons[0] = ui.file1Button;
    fileButtons[1] = ui.file2Button;
    QString  style = "QGroupBox {"
                     "border:1px solid #3A3939;"
                     "border-radius: 7px;"
                     "margin-top: 20px;"
                     "}"

                     "QGroupBox::title {"
                     "subcontrol-origin: margin;"
                     "subcontrol-position: top center;"
                     "padding-top: 10px;"
                     "}";
    ui.groupBox->setStyleSheet(style);
    ui.deleteButton->setStyleSheet("background-color: red;");
    QObject::connect(ui.fileGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, [=](int i){
        selected(id);
        fileSelected(i);
    });
    QObject::connect(ui.deleteButton, &QPushButton::clicked, this, [this]{ deletePressed(id); });
    setMouseTracking(true);
}

void PairWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit selected(id);
}
