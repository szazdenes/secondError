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
    errorMap.clear();
    sortedErrorMap.clear();
    ui->listWidget->clear();

    QDir dir("/home/denes/Documents/Labor/Viking/2ndStepSorter/data");
    QStringList fileList(dir.entryList(QStringList(), QDir::Files | QDir::NoDotAndDotDot));
    foreach(QString str, fileList){
        if(!str.startsWith(dir.path()))
            fileList.replace(fileList.indexOf(str), dir.absoluteFilePath(str));
    }

    foreach(QString currentFileName, fileList){
        QFile currentFile(currentFileName);
        if(!currentFile.open(QIODevice::ReadOnly | QIODevice::Text))
            qDebug() << "error while opening " + currentFileName;
        QTextStream stream(&currentFile);
        while(!stream.atEnd()){
            double a,b,c,d,e,f;
            QString key; /* delta, elev, th1, th2 */
            QPair<double, double> values; /* delev, dphi */
            QString line = stream.readLine();
            QTextStream text(&line);
            if(!line.startsWith("#")){
                text >> a >> b >> c >> d >> e >> f;
                key = QString::number(a) + " " + QString::number(b) + " " + QString::number(c) + " " + QString::number(d);
                values.first = e;
                values.second = f;
                errorMap[key].append(values);
            }
        }
        currentFile.close();
        QApplication::processEvents();
        emit signalWriteToList(currentFileName + " reading ready.");
    }

    double delta,elev,th1,th2;
    QList<int> deltastart, deltaend, elevstart, elevend, th1start, th1end, th2start, th2end;

    deltastart.append(35); deltastart.append(65); deltastart.append(95); deltastart.append(125);
    deltaend.append(55); deltaend.append(85); deltaend.append(115); deltaend.append(145);
    elevstart.append(5); elevstart.append(35);
    elevend.append(25); elevend.append(55);
    th1start.append(35); th1start.append(65); th1start.append(95);
    th1end.append(55); th1end.append(85); th1end.append(115);
    th2start.append(35); th2start.append(65); th2start.append(95);
    th2end.append(55); th2end.append(85); th2end.append(115);

    QMap<QString, int> counter;

    for(int i = 0; i < deltastart.size(); i++){
        for(int j = 0; j < elevstart.size(); j++){
            for(int k = 0; k < th1start.size(); k++){
                for(int l = 0; l < th2start.size(); l++){

                    counter[QString::number(deltastart.at(i)) + "-" + QString::number(deltaend.at(i)) + " " + QString::number(elevstart.at(j)) + "-" + QString::number(elevend.at(j)) + " " + QString::number(th1start.at(k)) + "-" + QString::number(th1end.at(k)) + " " + QString::number(th2start.at(l)) + "-" + QString::number(th2end.at(l))] = 0;

                    foreach (QString currentKey, errorMap.keys()){

                        QStringList splitKeys = currentKey.split(" ");
                        delta = QString(splitKeys.takeFirst()).toDouble();
                        elev = QString(splitKeys.takeFirst()).toDouble();
                        th1 = QString(splitKeys.takeFirst()).toDouble();
                        th2 = QString(splitKeys.takeFirst()).toDouble();

                        if(delta >= (double)deltastart.at(i) && delta <= (double)deltaend.at(i) && elev >= (double)elevstart.at(j) && elev <= (double)elevend.at(j) && th1 >= (double)th1start.at(k) && th1 <= (double)th1end.at(k) && th2 >= (double)th2start.at(l) && th2 <= (double)th2end.at(l)){
                            counter[QString::number(deltastart.at(i)) + "-" + QString::number(deltaend.at(i)) + " " + QString::number(elevstart.at(j)) + "-" + QString::number(elevend.at(j)) + " " + QString::number(th1start.at(k)) + "-" + QString::number(th1end.at(k)) + " " + QString::number(th2start.at(l)) + "-" + QString::number(th2end.at(l))]++;
                            QList<QPair<double, double> > currentErrorList = errorMap[currentKey];
                            for(int z = 0; z < currentErrorList.size(); z++)
                                sortedErrorMap[QString::number(deltastart.at(i)) + "-" + QString::number(deltaend.at(i)) + " " + QString::number(elevstart.at(j)) + "-" + QString::number(elevend.at(j)) + " " + QString::number(th1start.at(k)) + "-" + QString::number(th1end.at(k)) + " " + QString::number(th2start.at(l)) + "-" + QString::number(th2end.at(l))].append(currentErrorList.at(z));
                        }
                    }
                }
            }
        }
    }

    foreach(QString currkey, counter.keys()){
        if(counter[currkey] == 0)
            emit signalWriteToList(currkey + " is missing.");
        QApplication::processEvents();
    }

    emit signalWriteToList("Ready.");
}

QPair<double, double> secodStepSorter::getAverage(QList<QPair<double, double> > list)
{
    QPair<double, double> result(0,0);
    for (int i = 0; i < list.size(); i++){
        result.first += (double)list.at(i).first / (double)list.size();
        result.second += (double)list.at(i).second / (double)list.size();
    }
    return result;
}

QPair<double, double> secodStepSorter::getStD(QList<QPair<double, double> > list)
{
    QPair<double, double> result(0,0);
    QPair<double, double> average = getAverage(list);
    for (int i = 0; i < list.size(); i++){
        result.first += (((double)list.at(i).first - average.first)*((double)list.at(i).first - average.first)) / (double)list.size();
        result.second += (((double)list.at(i).second - average.second)*((double)list.at(i).second - average.second)) / (double)list.size();
    }
    result.first = qSqrt(result.first);
    result.second = qSqrt(result.second);
    return result;
}

void secodStepSorter::on_pushButton_2_clicked()
{
    if(errorMap.isEmpty() || sortedErrorMap.isEmpty())
        emit signalWriteToList("Data not calculated, press Start button.");

    else{

        QString filename(QFileDialog::getSaveFileName());
        QFile outfile1(filename + "_sit"), outfile2(filename + "_ranges");
        if(!outfile1.fileName().endsWith(".csv"))
            outfile1.setFileName(outfile1.fileName() + ".csv");
        if(!outfile2.fileName().endsWith(".csv"))
            outfile2.setFileName(outfile2.fileName() + ".csv");
        outfile1.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out1(&outfile1);
        outfile2.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out2(&outfile2);

        out1 << "#parameters\telev_error_ave\telev_error_std\tazimuth_error_ave\tazimuth_error_std\n";
        out2 << "#parameters\telev_error_ave\telev_error_std\tazimuth_error_ave\tazimuth_error_std\n";

        QMap<QString, QPair<double, double> > aveResultMap, aveSortedResultMap, stdResultMap, stdSortedResultMap;
        foreach(QString str, errorMap.keys()){
            aveResultMap[str] = getAverage(errorMap[str]);
            stdResultMap[str] = getStD(errorMap[str]);
            out1 << str + "\t" + QString::number(aveResultMap[str].first) + "\t" + QString::number(stdResultMap[str].first) + "\t" + QString::number(aveResultMap[str].second) + "\t" + QString::number(stdResultMap[str].second) + "\n";
        }
        foreach(QString str, sortedErrorMap.keys()){
            aveSortedResultMap[str] = getAverage(sortedErrorMap[str]);
            stdSortedResultMap[str] = getStD(sortedErrorMap[str]);
            out2 << str + "\t" + QString::number(aveSortedResultMap[str].first) + "\t" + QString::number(stdSortedResultMap[str].first) + "\t" + QString::number(aveSortedResultMap[str].second) + "\t" + QString::number(stdSortedResultMap[str].second) + "\n";
        }

        outfile1.close();
        outfile2.close();
    }

}
