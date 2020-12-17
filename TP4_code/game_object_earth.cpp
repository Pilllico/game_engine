#include "game_object_earth.h"

void GameEarthObject::update(){
    transform.rotate(3, QVector3D(0,1,0) );
    transform.rotateLocal(-2, QVector3D(0,1,0));
    updateChildren();
}
