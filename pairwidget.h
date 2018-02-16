#ifndef PAIRWIDGET_H
#define PAIRWIDGET_H

#include <QWidget>
#include <QString>
#include <QButtonGroup>
#include <QHash>

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
    QLabel *fileLabels[2];
    QRadioButton *fileButtons[2];
    QButtonGroup buttons;
    QHash<QString, QVariant> state[2];

public slots:

signals:
    void selected(const int id);
    void fileSelected(const int file);
    void deletePressed(const int id);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
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
};

#endif // PAIRWIDGET_H
