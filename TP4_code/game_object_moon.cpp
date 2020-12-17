#include "game_object_moon.h"

void GameMoonObject::update(){
    transform.rotateLocal(3, QVector3D(0,1,0));
    updateChildren();
}
