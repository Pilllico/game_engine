#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QOpenGLFunctions_3_1>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include "transform.h"

struct VertexData{
    QVector3D position;
    QVector2D texCoord;
};

class GameObject: public QOpenGLFunctions_3_1{
    public:
        GameObject();
        virtual ~GameObject();

        virtual void draw(QOpenGLShaderProgram *program);
        virtual void init();
        virtual void update();

        void setTexture(std::string texturePath){
            // Load image
            objTexture = new QOpenGLTexture(QImage(texturePath.c_str()).mirrored());

            // Set nearest filtering mode for texture minification
            objTexture->setMinificationFilter(QOpenGLTexture::Nearest);

            // Set bilinear filtering mode for texture magnification
            objTexture->setMagnificationFilter(QOpenGLTexture::Linear);

            // Wrap texture coordinates by repeating
            // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
            objTexture->setWrapMode(QOpenGLTexture::Repeat);

        }

        void addChild(GameObject *child){
            children.push_back(child);
            child->parent = this;
        }

        GameObject* getChild(int n){
            return children.at(n);
        }

        QMatrix4x4 getWorldMatrix(){
            if(parent != nullptr) return transform.combine_with(parent->getWorldMatrix());
            else return transform.m;
        }

        QMatrix4x4 getObjectMatrix(){
            return getWorldMatrix() * transform.ml;
        }

        Transform transform;

    protected:
        void initChildren(){for (unsigned long long i = 0; i < children.size(); i++ ) children[i]->init();}
        void updateChildren(){for (unsigned long long i = 0; i < children.size(); i++ ) children[i]->update();}
        void drawChildren(QOpenGLShaderProgram *program){for (unsigned long long i = 0; i < children.size(); i++ ) children[i]->draw(program);}

        int size;
        QOpenGLBuffer arrayBuf;
        QOpenGLBuffer indexBuf;
        std::vector<GameObject*> children;
        GameObject* parent = nullptr;
        QOpenGLTexture *objTexture = nullptr;
};

#endif // GAMEOBJECT_H
