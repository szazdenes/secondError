#ifndef SECODSTEPSORTER_H
#define SECODSTEPSORTER_H

#include <QMainWindow>
#include <QDir>
#include <QtDebug>
#include <QTextStream>
#include <QtMath>
#include <QFileDialog>

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

private:
    Ui::secodStepSorter *ui;
    QMap<QString, QList<QPair<double, double> > > errorMap, sortedErrorMap;
    QPair<double, double> getAverage(QList<QPair<double, double> > list);
    QPair<double, double> getStD(QList<QPair<double, double> > list);
};

#endif // SECODSTEPSORTER_H
