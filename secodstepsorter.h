#ifndef SECODSTEPSORTER_H
#define SECODSTEPSORTER_H

#define Pi 3.1415926536

#include <QMainWindow>
#include <QDir>
#include <QtDebug>
#include <QTextStream>
#include <QtMath>
#include <QFileDialog>
#include <QPainter>
#include <QPen>
#include <QVector2D>

#include "geomtransform.h"

namespace Ui {
class secodStepSorter;
}

class secodStepSorter : public QMainWindow
{
    Q_OBJECT

public:
    explicit secodStepSorter(QWidget *parent = 0);
    ~secodStepSorter();

signals:
    void signalWriteToList(QString string);

private slots:
    void slotWriteToList(QString string);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::secodStepSorter *ui;
    QMap<QString, QList<QPair<double, double> > > errorMap, sortedErrorMap;
    QPair<double, double> getAverage(QList<QPair<double, double> > list);
    QPair<double, double> getStD(QList<QPair<double, double> > list);

    void paint();
    GeomTransform *geom;
};

#endif // SECODSTEPSORTER_H
