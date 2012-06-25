#include "BasketballWidget.h"

#include <iostream>
using namespace std;

#include <QTimer>
#include <QtOpenGL>

#include "Sound.h"
#include "World.h"

GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fLightPos[]   = { 10.0f, 18.0f, 10.0f, 1.0f };

const char *szTextureFiles[] = {"texture/houston.tga", "texture/basketball.tga"};

BasketballWidget::BasketballWidget(QWidget *parent):
	QGLWidget(parent)
{
	/* empty */
}
void BasketballWidget::initializeGL()
{
	/* The init of the ball */
	m_world = new World();
	m_world->ball = BallModel(-10, 5, -3,            /* pos */
				  5, 1, 3,               /* velocity */
				  0, 0, 0,               /* rotation axis */
				  0.1, 0.5, 0, 0,        /* radius, weight, rotation, w velocity*/
				  0.75, 0.5, 0.5, 0.07,  /* coefficients */
				  0, 0, 0);              /* states */
	m_world->gravity = -9.8;
	m_world->air_density = 1.25;

	m_world->camera.pos[1] += m_world->ball.v[1];

	FlatModel ground = FlatModel(0, -0.3, 0,
				     20, 0.3, 20);
	ground.material = FlatModel::Ground;
	ground.visibleSurfaces = FlatModel::Top | FlatModel::Bottom;
	m_world->flats.append(ground);

	Light light0(fLightPos, fLowLight, fBrightLight, fBrightLight);
	m_world->lights.append(light0);

	m_sound = new Sound();
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	// Clear stencil buffer with zero, increment by one whenever anybody
	// draws into it. When stencil function is enabled, only write where
	// stencil value is zero. This prevents the transparent shadow from drawing
	// over itself
	glStencilOp(GL_INCR, GL_INCR, GL_INCR);
	glClearStencil(0);
	glStencilFunc(GL_EQUAL, 0x0, 0x01);

	// Clear stencil buffer with zero, increment by one whenever anybody
	// draws into it. When stencil function is enabled, only write where
	// stencil value is zero. This prevents the transparent shadow from drawing
	// over itself
	glStencilOp(GL_INCR, GL_INCR, GL_INCR);
	glClearStencil(0);
	glStencilFunc(GL_EQUAL, 0x0, 0x01);
    
	// Cull backs of polygons
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE_ARB);
    
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	// Calculate shadow matrix

	M3DVector3f vPoints[3] = {{ 0.0f, 0.3f, 0.0f },
				  { 10.0f, 0.3f, 0.0f },
				  { 5.0f, 0.4f, -5.0f }};

	M3DVector4f pPlane;
	m3dGetPlaneEquation(pPlane, vPoints[0], vPoints[1], vPoints[2]);
	m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, m_world->lights[0].pos.vec);

	// Mostly use material tracking
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
	glMateriali(GL_FRONT, GL_SHININESS, 128);


	// Set up texture maps
	glEnable(GL_TEXTURE_2D);
        glGenTextures(NUM_TEXTURES, textures);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
	// Load the texture objects
	for(int i = 0; i < NUM_TEXTURES; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);

		QImage image = QImage(szTextureFiles[i]);
		cout << image.width() << "S" << image.height() << endl;
		QImage texture = convertToGLFormat(image);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, texture.width(), texture.height(),
			     0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
        
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(10);
}

void BasketballWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, (GLsizei) w , (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h ==0 ) h = 1;
	gluPerspective(45.0f,(GLfloat) w / (GLfloat)h , 0.5f, -1000.0f);
//	glOrthof(-0.5, 0.5, 0.5, -0.5, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void BasketballWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	QGLWidget::renderText(100, 100, "Hi");

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);///清空颜色缓存和深度缓存

	glLoadIdentity();

	for (int i = 0; i != m_world->lights.size(); ++i) {
		GLint light;
		switch (i) {
		case 0:
			light = GL_LIGHT0; break;
		default:
			cout << "ERROR LIGHT " << i << endl;
		}
		glLightfv(light, GL_POSITION, m_world->lights[i].pos.vec);
		glLightfv(light, GL_AMBIENT, m_world->lights[i].ambient.vec);
		glLightfv(light, GL_DIFFUSE, m_world->lights[i].diffuse.vec);
		glLightfv(light, GL_SPECULAR, m_world->lights[i].specular.vec);
	}
	gluLookAt(0.0, 2.0, 0.0,
              0.0, -1.0, 0.0,
              0.0f, 1.0f,0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawSphere(20, 40, 20);

	for (int i = 0; i != m_world->flats.size(); ++i)
		drawFlat(m_world->flats[i]);
/*
	for (int i = 0; i != m_world->lights.size(); ++i) {
		GLint light;
		switch (i) {
		case 0:
			light = GL_LIGHT0; break;
		default:
			cout << "ERROR LIGHT " << i << endl;
		}
		glLightfv(light, GL_POSITION, m_world->lights[i].pos.vec);
		glLightfv(light, GL_AMBIENT, m_world->lights[i].ambient.vec);
		glLightfv(light, GL_DIFFUSE, m_world->lights[i].diffuse.vec);
		glLightfv(light, GL_SPECULAR, m_world->lights[i].specular.vec);
	}


	gluLookAt(0.0,0.0,5.0,
              0.0,0.0,-1.0,
              0.0f,1.0f,0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawSphere(0.1, 40, 20);
	
	if (m_world->camera.pos[1] > 2) {
		m_world->camera.pos[1] -= 0.05;
	}
	for (int i = 0; i != 3; ++ i) {
		cout << m_world->ball.pos[i] << endl;
	}
	gluLookAt(m_world->camera.pos[0] + m_world->ball.pos[0], 
	 	  m_world->camera.pos[1], 
	 	  m_world->camera.pos[2] + m_world->ball.pos[2], 
	 	  0.9 * m_world->ball.pos[0], 
	 	  -m_world->camera.pos[1], 
	 	  0.9 * m_world->ball.pos[2] ,
	 	  0, 1, 0);

	glTranslatef(0, 0, 10);

//	glDepthMask(GL_TRUE);

	for (int i = 0; i != m_world->flats.size(); ++i)
		drawFlat(m_world->flats[i]);

	glPushMatrix();
	glTranslatef(m_world->ball.pos[0], m_world->ball.pos[1], m_world->ball.pos[2]);

	if (m_world->ball.is_hit) {
		glScalef(1, 0.95, 1);
		m_sound->play("Hit");
	}

	glBindTexture(GL_TEXTURE_2D, textures[BALL_TEXTURE]);
        // Draw shadows first

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);
        glPushMatrix();
	glMultMatrixf(mShadowMatrix);
        glColor4f(0.00f, 0.00f, 0.00f, .6f);  // Shadow color
	drawSphere(m_world->ball.radius, 40, 20);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPopMatrix();
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

	glRotatef(m_world->ball.rotation, 
		  m_world->ball.ra[0], 
		  m_world->ball.ra[1], 
		  m_world->ball.ra[2]);

//	cout << ball.rotation << "\t" << ball.x << "\t" << ball.y << "\t" << ball.z << endl;

	glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
	drawSphere(m_world->ball.radius, 40, 20);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
   
	glPopMatrix();
*/

}

void BasketballWidget::drawFlat(const FlatModel &flat)
{
	switch (flat.material) {
	case FlatModel::Ground:
		glBindTexture(GL_TEXTURE_2D, textures[GROUND_TEXTURE]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case FlatModel::Wall:
		break;
	}

	GLfloat x_min = flat.x - flat.width;
	GLfloat x_max = flat.x + flat.width;
	GLfloat y_min = flat.y - flat.height;
	GLfloat y_max = flat.y + flat.height;
	GLfloat z_min = flat.z - flat.depth;
	GLfloat z_max = flat.z + flat.depth;

	glBegin(GL_QUADS);
        //front
	if (flat.visibleSurfaces & FlatModel::Front) {
		glTexCoord2f(0.0, 0.0);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x_min, y_max, z_max);
		glTexCoord2f(1.0, 0.0);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x_min, y_min, z_max);
		glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x_max, y_min, z_max);
		glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x_max, y_max, z_max);
	} else {
		glVertex3f(x_min, y_max, z_max);
		glVertex3f(x_min, y_min, z_max);
		glVertex3f(x_max, y_min, z_max);
		glVertex3f(x_max, y_max, z_max);
	}

        //bottom
	if (flat.visibleSurfaces & FlatModel::Bottom) {
		glTexCoord2f(0.0, 0.0);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(x_min, y_min, z_max);
		glTexCoord2f(1.0, 0.0);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(x_max, y_min, z_max);
		glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(x_max, y_min, z_min);
		glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(x_min, y_min, z_min);
	} else {
		glVertex3f(x_min, y_min, z_max);
		glVertex3f(x_max, y_min, z_max);
		glVertex3f(x_max, y_min, z_min);
		glVertex3f(x_min, y_min, z_min);
	}

        //back
	if (flat.visibleSurfaces & FlatModel::Back) {
		glTexCoord2f(1.0, 0.0);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(x_max, y_max, z_min);
		glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(x_max, y_min, z_min);
		glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(x_min, y_min, z_min);
		glTexCoord2f(0.0, 0.0);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(x_min, y_max, z_min);
	} else {
		glVertex3f(x_max, y_max, z_min);
		glVertex3f(x_max, y_min, z_min);
		glVertex3f(x_min, y_min, z_min);
		glVertex3f(x_min, y_max, z_min);
	}
    
        //top, all point up
	if (flat.visibleSurfaces & FlatModel::Top) {
		glTexCoord2f(0.0, 0.0);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x_min, y_max, z_max);
		glTexCoord2f(1.0, 0.0);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x_max, y_max, z_max);
		glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x_max, y_max, z_min);
		glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x_min, y_max, z_min);
	} else {
		glVertex3f(x_min, y_max, z_max);
		glVertex3f(x_max, y_max, z_max);
		glVertex3f(x_max, y_max, z_min);
		glVertex3f(x_min, y_max, z_min);
	}

        //left
	if (flat.visibleSurfaces & FlatModel::Left) {
		glTexCoord2f(1.0, 0.0);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(x_min, y_max, z_max);
		glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(x_min, y_min, z_max);
		glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(x_min, y_min, z_min);
		glTexCoord2f(0.0, 0.0);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(x_min, y_max, z_min);
	} else {
		glVertex3f(x_min, y_max, z_max);
		glVertex3f(x_min, y_min, z_max);
		glVertex3f(x_min, y_min, z_min);
		glVertex3f(x_min, y_max, z_min);
	}

        //right
	if (flat.visibleSurfaces & FlatModel::Right) {
		glTexCoord2f(1.0, 0.0);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(x_max, y_max, z_max);
		glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(x_max, y_min, z_max);
		glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(x_max, y_min, z_min);
		glTexCoord2f(0.0, 0.0);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(x_max, y_max, z_min);
	} else {
		glVertex3f(x_max, y_max, z_max);
		glVertex3f(x_max, y_min, z_max);
		glVertex3f(x_max, y_min, z_min);
		glVertex3f(x_max, y_max, z_min);
	}
 
        glEnd();
}

void BasketballWidget::timeFunc(int)
{
	m_world->go(1.0 / FRAMES);
	updateGL();
}

// void BasketballWidget::updateWorld(const World &world, WhatChanged changes)
// {
// }


/* this piece of code is from opengl superbible's gltools.cpp */
void BasketballWidget::drawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks)
{
	GLfloat drho = (GLfloat)(3.141592653589) / (GLfloat) iStacks;
	GLfloat dtheta = 2.0f * (GLfloat)(3.141592653589) / (GLfloat) iSlices;
	GLfloat ds = 1.0f / (GLfloat) iSlices;
	GLfloat dt = 1.0f / (GLfloat) iStacks;
	GLfloat t = 1.0f;	
	GLfloat s = 0.0f;
	GLint i, j;     // Looping variables
	
	for (i = 0; i < iStacks; i++) 
	{
		GLfloat rho = (GLfloat)i * drho;
		GLfloat srho = (GLfloat)(sin(rho));
		GLfloat crho = (GLfloat)(cos(rho));
		GLfloat srhodrho = (GLfloat)(sin(rho + drho));
		GLfloat crhodrho = (GLfloat)(cos(rho + drho));
		
		// Many sources of OpenGL sphere drawing code uses a triangle fan
		// for the caps of the sphere. This however introduces texturing 
		// artifacts at the poles on some OpenGL implementations
		glBegin(GL_TRIANGLE_STRIP);
		s = 0.0f;
		for ( j = 0; j <= iSlices; j++) 
		{
			GLfloat theta = (j == iSlices) ? 0.0f : j * dtheta;
			GLfloat stheta = (GLfloat)(-sin(theta));
			GLfloat ctheta = (GLfloat)(cos(theta));
			
			GLfloat x = stheta * srho;
			GLfloat y = ctheta * srho;
			GLfloat z = crho;
            
			glTexCoord2f(s, t);
			glNormal3f(x, y, z);
			glVertex3f(x * fRadius, y * fRadius, z * fRadius);
			
			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;
			glTexCoord2f(s, t - dt);
			s += ds;
			glNormal3f(x, y, z);
			glVertex3f(x * fRadius, y * fRadius, z * fRadius);
		}
		glEnd();

		t -= dt;
        }
}
