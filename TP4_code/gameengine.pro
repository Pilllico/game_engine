QT       += core gui widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

TARGET = gameengine
TEMPLATE = app

SOURCES += main.cpp \
    game_object.cpp \
    game_object_cube.cpp \
    game_object_earth.cpp \
    game_object_moon.cpp \
    game_object_sun.cpp \
    game_object_terrain.cpp \
    game_scene.cpp \
    transform.cpp

SOURCES +=

HEADERS += \
    game_object.h \
    game_object_cube.h \
    game_object_earth.h \
    game_object_moon.h \
    game_object_sun.h \
    game_object_terrain.h \
    game_scene.h \
    transform.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

# install
target.path = $$[YOUR_PATH]
INSTALLS += target
