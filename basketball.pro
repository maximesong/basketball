######################################################################
# Automatically generated by qmake (2.01a) Thu Mar 29 21:11:11 2012
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . lib src
INCLUDEPATH += . src/ src/lib

unix {
LIBS += -lglut -lGL -lGLU
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

SOURCES += lib/GLee.c lib/gltools.cpp lib/math3d.cpp src/basketball.cpp \
           src/modeling.cpp
