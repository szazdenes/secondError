#include "secodstepsorter.h"
#include "ui_secodstepsorter.h"

secodStepSorter::secodStepSorter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secodStepSorter)
{
    ui->setupUi(this);

    connect(this, SIGNAL(signalWriteToList(QString)), this, SLOT(slotWriteToList(QString)));
}

secodStepSorter::~secodStepSorter()
{
    delete ui;
}

void secodStepSorter::slotWriteToList(QString string)
{
    if(ui->listWidget->count() > 100)
        ui->listWidget->takeItem(0);

    ui->listWidget->addItem(string);
    ui->listWidget->scrollToBottom();
}

void secodStepSorter::on_pushButton_clicked()
{

}
