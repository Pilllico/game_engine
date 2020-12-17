#include "game_object.h"

#include <iostream>
#include <QVector2D>
#include <QVector3D>

using namespace std;

//! [0]
GameObject::GameObject(): indexBuf(QOpenGLBuffer::IndexBuffer){
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    if(objTexture == nullptr) setTexture(":/cube.png");

}

GameObject::~GameObject()
{
    arrayBuf.destroy();
    indexBuf.destroy();
    if(objTexture != nullptr) delete objTexture;
}

void GameObject::init(){
    qDebug()<< "gameObject init";
    initChildren();
}

void GameObject::draw(QOpenGLShaderProgram *program){

    if(objTexture != nullptr) objTexture->bind(0);

    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Set modelview-projection matrix
    program->setUniformValue("obj_matrix", getObjectMatrix());

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, size, GL_UNSIGNED_SHORT, 0);

    drawChildren(program);
}

void GameObject::update(){
    updateChildren();
}

