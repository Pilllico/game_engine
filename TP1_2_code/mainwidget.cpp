/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>

#include <iostream>

using namespace std;

float c_x, c_y, c_z;
float init_depth = 100.0;
bool free_camera = false;

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    texture_map(0),
    texture_grass(0),
    texture_rock(0),
    texture_snow(0),
    angularSpeed(0)
{
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture_map;
    delete texture_grass;
    delete texture_rock;
    delete texture_snow;
    delete geometries;
    doneCurrent();
}

void MainWidget::keyPressEvent(QKeyEvent *e){

    float d_x = 1.0f;
    float d_y = 1.0f;
    float d_z = 1.0f;

    if (free_camera){
        if (e->text() == QString::fromStdString("a")) c_z -= d_z;
        if (e->text() == QString::fromStdString("e")) c_z += d_z;

        if (e->text() == QString::fromStdString("d")) c_x -= d_x;
        if (e->text() == QString::fromStdString("q")) c_x += d_x;

        if (e->text() == QString::fromStdString("z")) c_y -= d_y;
        if (e->text() == QString::fromStdString("s")) c_y += d_y;
    }

    if (e->text() == QString::fromStdString("c")) free_camera = !free_camera;

    update();
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;
    mouse_rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * mouse_rotation;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
        const_rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), 1.0f) * const_rotation;
        update();
    } else {
        // Update rotation
        mouse_rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * mouse_rotation;
        update();
    }
}
//! [1]

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);
//! [2]

    geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

//! [3]
void MainWidget::initShaders()
{
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
//! [3]

//! [4]
void MainWidget::initTextures()
{
    // Load texture.png image
    texture_map = new QOpenGLTexture(QImage(":/ressources/heightmap-1024x1024.png").mirrored());
    texture_grass = new QOpenGLTexture(QImage(":/ressources/grass.png").mirrored());
    texture_rock = new QOpenGLTexture(QImage(":/ressources/rock.png").mirrored());
    texture_snow = new QOpenGLTexture(QImage(":/ressources/snowrocks.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture_map->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_grass->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_rock->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_snow->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture_map->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_grass->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_rock->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_snow->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture_map->setWrapMode(QOpenGLTexture::Repeat);
    texture_grass->setWrapMode(QOpenGLTexture::Repeat);
    texture_rock->setWrapMode(QOpenGLTexture::Repeat);
    texture_snow->setWrapMode(QOpenGLTexture::Repeat);

}
//! [4]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 1.0, far plane to 500.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 500.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture_map->bind(0);
    texture_grass->bind(1);
    texture_rock->bind(2);
    texture_snow->bind(3);

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;

    if (!free_camera){
        matrix.translate(c_x, c_y, c_z - init_depth);
        matrix.rotate(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), -60.0f));
        matrix.rotate(const_rotation);
    } else {
        matrix.translate(c_x, c_y - 3, c_z);
        matrix.rotate(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), -90.0f));
        matrix.rotate(mouse_rotation);
    }

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);
//! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture_map", 0);
    program.setUniformValue("texture_grass", 1);
    program.setUniformValue("texture_rock", 2);
    program.setUniformValue("texture_snow", 3);

    // Draw plane geometry
    geometries->drawPlaneGeometry(&program);
}
