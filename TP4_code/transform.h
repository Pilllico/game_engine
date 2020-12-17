#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QVector3D>
#include <QMatrix4x4>
#include <iostream>

using namespace std;

class Transform{

public:
    Transform();
    Transform(QMatrix4x4 mat);

    QMatrix4x4 m;
    QMatrix4x4 ml;

    void translate( QVector3D p);
    void rotate( float angle, QVector3D p);
    void rescale( QVector3D p);
    void translateLocal( QVector3D p);
    void rotateLocal( float angle, QVector3D p);
    void rescaleLocal( QVector3D p);

    QMatrix4x4 combine_with (QMatrix4x4 t);
    Transform inverse();
    Transform interpolate_with(Transform& t, float k);

    static QVector4D lerp( const QVector4D& A, const QVector4D& B, float t );
};

#endif // TRANSFORM_H
