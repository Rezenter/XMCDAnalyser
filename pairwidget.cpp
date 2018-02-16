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
    for(int i = 0; i < 2; ++i){
        buttons.addButton(fileButtons[i], i);
    }
    ui.groupBox->setStyleSheet(style);
    ui.deleteButton->setStyleSheet("background-color: red;");
    QObject::connect(&buttons, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), this, [=](int i, bool state){
        if(state){
            selected(id);
            fileSelected(i);
        }
    });
    QObject::connect(ui.deleteButton, &QPushButton::clicked, this, [this]{ deletePressed(id);});
    setMouseTracking(true);
}

void PairWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit selected(id);
}
