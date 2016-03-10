#include "geomtransform.h"

GeomTransform::GeomTransform(QObject *parent) : QObject(parent)
{

}

QVector3D GeomTransform::descartes2Polar(QVector3D inputDescartes)
{
    QVector3D result;
    result.setX( inputDescartes.length());
    result.setY( acos(inputDescartes.z()));
    result.setZ( atan2(inputDescartes.y(), inputDescartes.x()) );
    return result;
}

QVector3D GeomTransform::polar2Descartes(QVector3D inputPolar)
{
    QVector3D result;
    result.setX( inputPolar.x() * sin(inputPolar.y()) * cos(inputPolar.z()) );
    result.setY( inputPolar.x() * sin(inputPolar.y()) * sin(inputPolar.z()) );
    result.setZ( inputPolar.x() * cos(inputPolar.y()));
    return result;
}

QVector2D GeomTransform::polar2Fisheye(QVector3D inputPolar)
{
    QVector2D result;
    result.setX(inputPolar.y() * cos(inputPolar.z()));
    result.setY(inputPolar.y() * sin(inputPolar.z()));
    return result;
}

QVector2D GeomTransform::fisheye2Draw(QVector2D inputfisheye, double width)
{
    QVector2D result;
    result.setX(width/2.0 + inputfisheye.x() * width/Pi );
    result.setY(width/2.0 - inputfisheye.y() * width/Pi );
    return result;
}

QVector2D GeomTransform::draw2Fisheye(QVector2D inputdraw, double width)
{
    QVector2D result;
    result.setX((inputdraw.x() - width/2.0)*(Pi/width));
    result.setY((width/2.0 - inputdraw.y())*(Pi/width));
    return result;
}

QVector3D GeomTransform::fisheye2Polar(QVector2D inputFisheye)
{
    QVector3D result;
    result.setX(1.0);
    result.setY(inputFisheye.length());
    result.setZ(atan2(inputFisheye.y(), inputFisheye.x()));
    return result;
}

QVector2D GeomTransform::descartes2Fisheye(QVector3D inputDescartes)
{
    return polar2Fisheye(descartes2Polar(inputDescartes));
}

QVector3D GeomTransform::fisheye2Descartes(QVector2D inputFisheye)
{
    return polar2Descartes(fisheye2Polar(inputFisheye));
}

QVector3D GeomTransform::rotate(QVector3D v, QVector3D axis, double alpha)
{
    QVector3D result;
    double a = cos(alpha);
    double b = 1-a;
    double c = sin(alpha);
    result.setX(v.x()*(a+b*axis.x()*axis.x())+v.y()*(b*axis.x()*axis.y()-c*axis.z())+v.z()*(b*axis.x()*axis.z()+c*axis.y()));
    result.setY(v.x()*(b*axis.x()*axis.y()+c*axis.z())+v.y()*(a+b*axis.y()*axis.y())+v.z()*(b*axis.y()*axis.z()-c*axis.x()));
    result.setZ(v.x()*(b*axis.x()*axis.z()-c*axis.y())+v.y()*(b*axis.y()*axis.z()+c*axis.x())+v.z()*(a+b*axis.z()*axis.z()));
    return result;
}

QVector2D GeomTransform::rotate2D(QVector2D v, double alpha)
{
    QVector2D result;
    result.setX(v.x()*cos(alpha) - v.y()*sin(alpha));
    result.setY(v.x()*sin(alpha) + v.y()*cos(alpha));
    return result;
}

