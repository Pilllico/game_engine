#include "transform.h"

Transform::Transform(){
    m = QMatrix4x4();
    ml = QMatrix4x4();
}

Transform::Transform(QMatrix4x4 mat){
    ml = m = mat;
}


void Transform::translate( QVector3D p){
    m.translate(p);
}

void Transform::rotate( float angle, QVector3D p){
    m.rotate(angle, p);
}

void Transform::rescale( QVector3D p){
    m.scale(p);
}

void Transform::translateLocal( QVector3D p){
    ml.translate(p);
}

void Transform::rotateLocal( float angle, QVector3D p){
    ml.rotate(angle, p);
}

void Transform::rescaleLocal( QVector3D p){
    ml.scale(p);
}


QMatrix4x4 Transform::combine_with (QMatrix4x4 t){
    return t * m;
}

Transform Transform::inverse(){
    return Transform(m.inverted());
}

Transform Transform::interpolate_with(Transform& t, float k){
    Transform result = Transform();

    QVector4D translation = m.column(3);
    QVector4D toTranslation = t.m.column(3);
    result.m.setColumn(3, Transform::lerp(translation, toTranslation, k));

    return result;
}

QVector4D Transform::lerp( const QVector4D& A, const QVector4D& B, float t ){
  return A*t + B*(1.f-t) ;
}
