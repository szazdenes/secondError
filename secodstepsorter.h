#ifndef SECODSTEPSORTER_H
#define SECODSTEPSORTER_H

#include <QMainWindow>
#include <QDir>
#include <QtDebug>
#include <QTextStream>

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

private:
    Ui::secodStepSorter *ui;
    QMap<QString, QList<QPair<double, double> > > errorMap, sortedErrorMap;
};

#endif // SECODSTEPSORTER_H
