#include "game_object_sun.h"

void GameSunObject::update(){
    transform.rotate(-2, QVector3D(0,1,0));
    transform.rotateLocal(1, QVector3D(0,1,0));
    updateChildren();
}
