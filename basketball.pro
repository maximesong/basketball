######################################################################
# Automatically generated by qmake (2.01a) Thu Mar 29 21:11:11 2012
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . lib src
INCLUDEPATH += . src/ src/lib
QT += opengl

#SOURCES += src/main.cpp  src/BasketballWidget.cpp 
SOURCES += src/basketball.cpp src/lib/gltools.cpp

unix {
LIBS += -lglut -lGLU
QT += phonon opengl
config += debug
}

win32 {
LIBS += ./lib/glut32.lib ./lib/glu32.lib ./lib/opengl32.lib 
CONFIG += release
}

unix {
CONFIG += nas
}

OBJECTS_DIR += ./objs
MOC_DIR += ./tmp

# Input
HEADERS += lib/GLee.h lib/glfrustum.h lib/gltools.h lib/math3d.h \
           src/modeling.h src/basketball.h 

SOURCES += lib/GLee.c  lib/math3d.cpp  \
           src/modeling.cpp



HEADERS += Sound.h \
           World.h \
           Vector.h \
           Light.h \
           BallModel.h \
           FlatModel.h \

SOURCES += Sound.cpp \
           World.cpp \
           Vector.cpp \
           Light.cpp \
           BallModel.cpp \
           FlatModel.cpp \
