#include "game_scene.h"

#include <QMouseEvent>

#include <math.h>
#include <iostream>

using namespace std;

GameScene::GameScene(QWidget *parent) :QOpenGLWidget(parent),gameObject(0),textureGrass(0),textureRock(0),textureSnow(0),textureMap(0),angularSpeed(0){
    xOffset = 0;
    yOffset = -1;
    zOffset = 0;

}

GameScene::~GameScene(){
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete gameObject;
    doneCurrent();
}

void GameScene::keyPressEvent(QKeyEvent *e){
    if(freeView){
        //if(e->key() == Qt::Key_W || e->key() == Qt::Key_Z){zOffset-=1.0f;}
        //if(e->key() == Qt::Key_S){zOffset+=1.0f;}
        //if(e->key() == Qt::Key_Space){yOffset-=1.0f;}
        //if(e->key() == Qt::Key_Shift){yOffset+=1.0f;}
    }
    if(e->key() == Qt::Key_C){freeView = !freeView;}
    if(e->key() == Qt::Key_A || e->key() == Qt::Key_Q){cube->transform.translate(QVector3D(-0.25f,0,0));}
    if(e->key() == Qt::Key_D){cube->transform.translate(QVector3D(0.25f,0,0));}
    if(e->key() == Qt::Key_Z || e->key() == Qt::Key_W){cube->transform.translate(QVector3D(0,0,-0.25f));}
    if(e->key() == Qt::Key_S){cube->transform.translate(QVector3D(0,0,0.25f));}

    int n = terrain->length;

    int x = cube->transform.m.column(3).x() * (n / 32) + n / 2.0f;
    int z = n - (cube->transform.m.column(3).z() * (n / 32) + n / 2.0f);

    if (0 <= x && x < n && 0 <= z && z < n){
        float y = (terrain->vertices.at(z * n + x).position.y()) - get_min() + cube->offset;
        cube->transform.translate(QVector3D(0, y - cube->transform.m.column(3).y(),0));
    }

    updateScene();
}

void GameScene::mousePressEvent(QMouseEvent *e){
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void GameScene::mouseReleaseEvent(QMouseEvent *e){
    if(freeView){
        // Mouse release position - mouse press position
        QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

        // Rotation axis is perpendicular to the mouse position difference
        // vector
        QVector3D n = QVector3D(0.0, diff.x(), 0.0).normalized();

        // Accelerate angular speed relative to the length of the mouse sweep
        qreal acc = diff.length() / 10.0;

        // Calculate new rotation axis as weighted sum
        rotationAxis = ( rotationAxis * angularSpeed + n * acc).normalized();
        /*
        rotationAxis[0] = 0;
        rotationAxis[2] = 0;
        */
        // Increase angular speed
        angularSpeed += acc;
    }
}

void GameScene::timerEvent(QTimerEvent *){
    if(freeView){
        // Decrease angular speed (friction)
        angularSpeed *= 0.50;

        // Stop rotation when speed goes below threshold
        if (angularSpeed < 0.01) {
            angularSpeed = 0.0;
        } else {
            // Update rotation
            rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

            // Request an update

        }
    }
    updateScene();

}

float GameScene::get_min(){
    float min_y = 1000000.0f;
    for(int i = 0; i < terrain->length; ++i){
        for (int j = 0; j < terrain->length; ++j) {
            float y = terrain->vertices.at(i * terrain->length + j).position.y();
            if (y < min_y) min_y = y;
        }
    }
    return min_y;
}

void GameScene::initializeGL(){
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();


    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    gameObject = new GameObject;

    //Child1

    terrain = new GameTerrainObject;
    terrain->setTexture(":/grass.png");
    terrain->transform.translate(QVector3D(0,0,0));
    terrain->transform.rescale(QVector3D(1,1,1));
    terrain->length = 256;

    cube = new GameCubeObject;
    int height = 5;
    cube->setTexture(":/sun.png");
    cube->transform.translate(QVector3D(0,height,0));
    cube->transform.rescale(QVector3D(0.5,0.5,0.5));
    cube->offset = height;

/*
    GameSunObject* sun = new GameSunObject;
    sun->setTexture(":/sun.png");
    sun->transform.rescale(QVector3D(1.5,1.5,1.5));

    GameEarthObject* earth = new GameEarthObject;
    earth->setTexture(":/earth.png");
    earth->transform.rescale(QVector3D(0.5,0.5,0.5));
    earth->transform.translate(QVector3D(10,0,0));

    GameMoonObject* moon = new GameMoonObject;
    moon->setTexture(":/moon.png");
    moon->transform.rescale(QVector3D(0.3,0.3,0.3));
    moon->transform.translate(QVector3D(10,0,0));

    earth->addChild(moon);
    sun->addChild(earth);
    gameObject->addChild(sun);
*/

    gameObject->addChild(terrain);
    gameObject->addChild(cube);


    //Init
    gameObject->init();
    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}


void GameScene::initShaders(){
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}


void GameScene::resizeGL(int w, int h){
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 70.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void GameScene::updateScene(){
    gameObject->update();
    update();
}

void GameScene::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1,0.1,0.2,1);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    if(freeView){
        matrix.translate(0, -5, -20);
        matrix.rotate(rotation);
        matrix.translate(xOffset, yOffset, zOffset);
    }else{
        matrix.rotate(QQuaternion::fromAxisAndAngle(1,0,0,90));
        matrix.translate(0, -50, 0);
    }

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("textureGrass", 0);
    program.setUniformValue("textureRock", 1);
    program.setUniformValue("textureSnow", 2);
    program.setUniformValue("textureMap", 3);

    gameObject->draw(&program);
}
