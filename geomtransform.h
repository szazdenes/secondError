#ifndef GEOMTRANSFORM_H
#define GEOMTRANSFORM_H

#define Pi 3.1415926536

#include <QObject>
#include <QWidget>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

class GeomTransform : public QObject
{
    Q_OBJECT
public:
    explicit GeomTransform(QObject *parent = 0);

    QVector3D descartes2Polar(QVector3D inputDescartes);
    QVector3D polar2Descartes(QVector3D inputPolar);
    QVector2D polar2Fisheye(QVector3D inputPolar);
    QVector2D fisheye2Draw(QVector2D inputfisheye, double width);
    QVector2D draw2Fisheye(QVector2D inputdraw, double width);
    QVector3D fisheye2Polar(QVector2D inputFisheye);
    QVector2D descartes2Fisheye(QVector3D inputDescartes);
    QVector3D fisheye2Descartes(QVector2D inputFisheye);
    QVector3D rotate(QVector3D v, QVector3D axis, double alpha);
    QVector2D rotate2D(QVector2D v, double alpha);

signals:

public slots:
};

#endif // GEOMTRANSFORM_H
