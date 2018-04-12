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
            log(QString(this->metaObject()->className()) + ":: toggled" + ". i == " + QString::number(i) + ". state == " + state);
            selected();
            fileSelected(i);
        }
    });
    QObject::connect(ui.deleteButton, &QPushButton::clicked, this, [=]{
        log(QString(this->metaObject()->className()) + ":: dalete pressed");
        deletePressed();
    });
    setMouseTracking(true);
}

void PairWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    log(QString(this->metaObject()->className()) + ":: mousePressEvent");
    emit selected();
}
