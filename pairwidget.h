#ifndef PAIRWIDGET_H
#define PAIRWIDGET_H

#include <QWidget>
#include <QString>

#include "ui_pairwidget.h"

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPaintEvent;
QT_END_NAMESPACE

class PairWidget : public QWidget
{
    Q_OBJECT

public:
    PairWidget(QWidget *parent = 0);
    PairWidget(const int index);
    Ui::Form ui;
    int id;
    QString state[2];
    QLabel *fileLabels[2];
    QRadioButton *fileButtons[2];

public slots:

signals:
    void selected(const int id);
    void fileSelected(const int file);
    void deletePressed(const int id);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // PAIRWIDGET_H
