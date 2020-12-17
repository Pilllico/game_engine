#include "game_object_terrain.h"
#include <QtDebug>

using namespace std;

void GameTerrainObject::init(){

    int size = length;

    height_map = QImage(":/heightmap.png", "png");

    for(int i = 0; i < size; ++i){
        for (int j = 0; j < size; ++j) {
            float y = (height_map.pixelColor(i, j).red() * 4.0f) / 256.0f;
            //if (i == 0 && j == 0){ y *= 5; }
            vertices.push_back({QVector3D(
                                   (j - size / 2.0f) * 32 / size,
                                    y,
                                   -(i - size / 2.0f) * 32 / size
                                ),
                                QVector2D(
                                (float)i/(size-1), (float)j/(size-1)
                                )});
        }
    }

    // heightPos.y = texture2D(texture_map, QVector2D((float)i/(size-1), (float)j/(size-1))).x;

    QVector<GLushort> indices;
    for (int i = 0; i < size-1; ++i) {
        if(i != 0){
            indices.push_back( i*size+size ); //beggin duplication
            //cout << indices[indices.size()-1] << " ";
        }
        for (int j = 0; j < size; ++j) {
            indices.push_back(i*size+j+size); //lower one
            //cout << indices[indices.size()-1] << " ";

            indices.push_back(i*size+j); //top one
            //cout << indices[indices.size()-1] << " ";

        }
        if(i != size-2){
            indices.push_back( i*size+size-1); //end duplication
            //cout << indices[indices.size()-1] << " " << endl;
        }
    }
    this->size = indices.size();

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices.data(), vertices.size() * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices.data(), indices.size() * sizeof(GLushort));

    initChildren();
}
