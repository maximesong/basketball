#ifndef _CARTOONSHADEWIDGET_H_
#define _CARTOONSHADEWIDGET_H_

//#include "gltools.h"
#include <QGLWidget>

#ifdef __linux__
#include "GL/glut.h"
#endif

#include "math3d.h"

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#define GROUND_TEXTURE  0
#define BALL_TEXTURE   1
#define NUM_TEXTURES    2
#define FRAMES 33

class Sound;
class World;
class FlatModel;

class BasketballWidget : public QGLWidget {
	Q_OBJECT
public:
	BasketballWidget(QWidget *parent = 0);
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void timeFunc(int);
public slots:
//	void updateWorld(const World &world, WhatChanged changes);
private:
        void drawFlat(const FlatModel &flat);
void drawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks);
	static const int TEXTURES_NR = 10;
	GLuint textures[TEXTURES_NR];

	GLfloat eyePos[3];
	GLfloat lightPos[4];
	GLfloat lights[4];

	const char *szTextureFiles[5];
	M3DMatrix44f mShadowMatrix;

	Sound *m_sound;
	World *m_world;
};

#endif /* _CARTOONSHADEWIDGET_H_ */
