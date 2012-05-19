#ifndef _CARTOONSHADEWIDGET_H_
#define _CARTOONSHADEWIDGET_H_

#include <QGLWidget>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <QSound>
#ifdef __linux__
#include <Phonon>
//#include <phonon/MediaSource>
#endif

#include "gltools.h"
#include "math3d.h"

#define GROUND_TEXTURE  0
#define BALL_TEXTURE   1
#define NUM_TEXTURES    2
#define FRAMES 33


class BasketballWidget : public QGLWidget {
	Q_OBJECT
	public:
	BasketballWidget(QWidget *parent = 0);
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
private:
	static const int TEXTURES_NR = 10;
	GLuint textures[TEXTURES_NR];

	GLfloat eyePos[3];
	GLfloat lightPos[4];
	GLfloat lights[4];

	const char *szTextureFiles[5];
	M3DMatrix44f mShadowMatrix;
	Ball ball;
	Env env;
};

#endif /* _CARTOONSHADEWIDGET_H_ */
