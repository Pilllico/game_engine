#ifndef GAMECUBEOBJECT_H
#define GAMECUBEOBJECT_H

#include "game_object.h"

class GameCubeObject : public GameObject{

public:

    int offset;
    void init();
    void update();

};

#endif // GAMECUBEOBJECT_H
