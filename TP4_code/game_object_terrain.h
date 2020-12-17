#ifndef GAMETERRAINOBJECT_H
#define GAMETERRAINOBJECT_H

#include "game_object.h"

class GameTerrainObject : public GameObject{

    public:
        int length;
        QVector<VertexData> vertices;

        void init();
        void get_min();

    protected:

    private:
        QImage height_map;
        QOpenGLTexture *texture_map;
        QOpenGLTexture *texture_grass;
        QOpenGLTexture *texture_rock;
        QOpenGLTexture *texture_snow;

};

#endif // GAMETERRAINOBJECT_H
