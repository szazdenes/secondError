#include "secodstepsorter.h"
#include "ui_secodstepsorter.h"

secodStepSorter::secodStepSorter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secodStepSorter)
{
    ui->setupUi(this);

    geom = new GeomTransform();
    connect(this, SIGNAL(signalWriteToList(QString)), this, SLOT(slotWriteToList(QString)));
}

secodStepSorter::~secodStepSorter()
{
    delete geom;
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

    int numAll = 0, numNoAntiSun = 0;
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
                numAll++;
                if(values.second < 90 && values.second > -90){
                    errorMap[key].append(values);
                    numNoAntiSun++;
                }
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
    emit signalWriteToList("No antisun ratio: " + QString::number((double)numNoAntiSun / (double)numAll));
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

void secodStepSorter::paint()
{
    QPainter painter;
    QPen pen;

    double size = 1000;

    QImage im=QImage(size,size,QImage::Format_ARGB32);
    painter.begin(&im);

    painter.fillRect(0,0,size,size,Qt::white);
    painter.drawEllipse(QPointF(size/2.0,size/2.0),3,3);
    painter.drawEllipse(QPointF(size/2.0,size/2.0),5,5);
    painter.drawEllipse(QPointF(size/2.0,size/2.0),size/2, size/2);

    QVector2D sunPos;
    QList<QVector2D> points;

    for(int i = 0; i < im.width(); i++){
        for(int j = 0; j < im.height(); j++){
            if(QVector2D(i-size/2.0, size/2.0 - j).length() <= (size/2.0)){
                points.append(QVector2D(i,j));
            }
        }
    }

    double count = 0;

    foreach(QString str, errorMap.keys()){
        double delta, elev, th1, th2;

        QStringList splitKeys = str.split(" ");
        delta = QString(splitKeys.takeFirst()).toDouble();
        elev = QString(splitKeys.takeFirst()).toDouble();
        th1 = QString(splitKeys.takeFirst()).toDouble();
        th2 = QString(splitKeys.takeFirst()).toDouble();

        sunPos = geom->fisheye2Draw(QVector2D(Pi/2.0-Pi*elev/180.0,0), size);
        pen.setColor(Qt::red);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawPoint(sunPos.x(), sunPos.y());

        QVector3D sunDescartes = geom->fisheye2Descartes(geom->draw2Fisheye(sunPos, size));

        pen.setColor(Qt::blue);
        pen.setWidth(1);
        painter.setPen(pen);

        QList<QVector3D> p1, p2;

        for(int i = 0; i < points.size(); i++){
            QVector3D currentp1 = geom->fisheye2Descartes(geom->draw2Fisheye(points.at(i), size));
            if((acos(QVector3D::dotProduct(currentp1.normalized(), sunDescartes.normalized())) * 180.0/Pi) >= th1-0.5 && (acos(QVector3D::dotProduct(currentp1.normalized(), sunDescartes.normalized())) * 180.0/Pi) < th1+0.5)
                p1.append(currentp1);
            QVector3D currentp2 = geom->fisheye2Descartes(geom->draw2Fisheye(points.at(i), size));
            if((acos(QVector3D::dotProduct(currentp2.normalized(), sunDescartes.normalized())) * 180.0/Pi) >= th2-0.5 && (acos(QVector3D::dotProduct(currentp2.normalized(), sunDescartes.normalized())) * 180.0/Pi) < th2+0.5)
                p2.append(currentp2);
        }

        for(int i = 0; i < p1.size(); i++){
            for(int j = 0; j < p2.size(); j++){
                QVector3D v1 = QVector3D::crossProduct(p1.at(i), sunDescartes).normalized();
                QVector3D v2 = QVector3D::crossProduct(p2.at(j), sunDescartes).normalized();
                if((acos(QVector3D::dotProduct(v1.normalized(), v2.normalized())) * 180.0/Pi) >= delta-0.5 && (acos(QVector3D::dotProduct(v1.normalized(), v2.normalized())) * 180.0/Pi) < delta + 0.5){
                    QVector2D p1Draw = geom->fisheye2Draw(geom->descartes2Fisheye(p1.at(i)), size);
                    QVector2D p2Draw = geom->fisheye2Draw(geom->descartes2Fisheye(p2.at(j)), size);
                    painter.drawPoint(p1Draw.x(), p1Draw.y());
                    painter.drawPoint(p2Draw.x(), p2Draw.y());
                }
            }
            QApplication::processEvents();
        }


        count++;
        emit signalWriteToList("Image " + QString::number(100*count/240.0) + " % ready.");
        QApplication::processEvents();

    }

    painter.end();
    im.save("a.png");
    emit signalWriteToList("Image saved.");

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

void secodStepSorter::on_pushButton_3_clicked()
{
    if(errorMap.isEmpty() || sortedErrorMap.isEmpty())
        emit signalWriteToList("Data not calculated, press Start button.");
    else{
        paint();
    }

}
