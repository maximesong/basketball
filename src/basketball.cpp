#include "basketball.h"

#include <iostream>
using namespace std;

#include "modeling.h"

GLfloat eyex=2,eyey=2,eyez=15;

Ball ball;
Env env;

#define FRAMES 33

GLuint textures[2];


void drawBox(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat d)
{
//	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	glBegin(GL_QUADS);
        //front
        glVertex3f(x-w, y+h, z+d);
        glVertex3f(x-w, y-h, z+d);
        glVertex3f(x+w, y-h, z+d);
        glVertex3f(x+w, y+h, z+d);

        //bottom
        glVertex3f(x-w, y-h, z+d);
        glVertex3f(x+w, y-h, z+d);
        glVertex3f(x+w, y-h, z-d);
        glVertex3f(x-w, y-h, z-d);

        //back
        glVertex3f(x+w, y+h, z-d);
        glVertex3f(x+w, y-h, z-d);
        glVertex3f(x-w, y-h, z-d);
        glVertex3f(x-w, y+h, z-d);
    
        //top
	glTexCoord2f(0.0, 0.0);
        glVertex3f(x-w, y+h, z+d);
	glTexCoord2f(1.0, 0.0);
        glVertex3f(x+w, y+h, z+d);
	glTexCoord2f(1.0, 1.0);
        glVertex3f(x+w, y+h, z-d);
	glTexCoord2f(0.0, 1.0);
        glVertex3f(x-w, y+h, z-d);

        //left
        glVertex3f(x-w, y+h, z+d);
        glVertex3f(x-w, y-h, z+d);
        glVertex3f(x-w, y-h, z-d);
        glVertex3f(x-w, y+h, z-d);

        //right
        glVertex3f(x+w, y+h, z+d);
        glVertex3f(x+w, y-h, z+d);
        glVertex3f(x+w, y-h, z-d);
        glVertex3f(x+w, y+h, z-d);
 
        glEnd();
}

void init()
{

	glClearColor(0.0, 0.0, 0.0, 0.0);

	GLbyte *pBytes;
        GLint nWidth, nHeight, nComponents;
        GLenum format;

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glGenTextures(2, textures);
        
	// Load the texture objects
        pBytes = gltLoadTGA("texture/basketball.tga", &nWidth, &nHeight, &nComponents, &format);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0,
		     format, GL_UNSIGNED_BYTE, pBytes);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	free(pBytes);

        pBytes = gltLoadTGA("texture/houston.tga", &nWidth, &nHeight, &nComponents, &format);

        glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0,
		     format, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	/* The init of the ball */

	ball.x = -10;
	ball.y = 5;
	ball.z = -3;
	ball.r = 0.1;
	ball.vx = 5;
	ball.vy = 1.0;
	ball.vz = 3.0;
	ball.weight = 0.5;
	ball.restitution_coefficient = 0.75;
	ball.drag_coefficient = 0.5;
	ball.m_coefficient = 0.5;
	ball.rolling_coefficient = 0.07;
	ball.is_almost_rolling = 0;
	ball.is_rolling = 0;
	ball.rotation = 0;

	env.gravity = -9.8;
	env.air_density = 1.25;

	eyey += ball.y;

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (eyey > 2) {
		eyey -= 0.05;
	}
	gluLookAt(eyex + ball.x, eyey, eyez + ball.z, 0.9 * ball.x, -eyey, 0.9 * ball.z ,0, 1, 0);

	glTranslatef(0, 0, 10);

	drawBox(0, -0.3, 0, 28, 0.3, 15);

	glDepthMask(GL_TRUE);

	glPushMatrix();
	glTranslatef(ball.x, ball.y, ball.z);
	if (ball.is_hit) {
		glScalef(1, 0.95, 1);
		QSound::play("music/hit.wav");
	}


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glRotatef(ball.rotation, ball.rx, ball.ry, ball.rz);

//	cout << ball.rotation << "\t" << ball.rx << "\t" << ball.ry << "\t" << ball.rz << endl;

	glBegin(GL_QUADS);
	GLUquadricObj *quadObj = gluNewQuadric();
	gluQuadricTexture(quadObj,GL_TRUE);
	gluSphere(quadObj, ball.r, 50, 30);
	gluDeleteQuadric(quadObj);
	glEnd(); 
   
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w , (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h ==0 ) h = 1;
	gluPerspective(45.0f,(GLfloat) w / (GLfloat)h , 0.5f, -1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunction(int)
{
	ball = calculateXyFallingBall(ball, env, 1.0 / FRAMES);
	glutPostRedisplay();
	glutTimerFunc(FRAMES, TimerFunction, 1);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Basketball Demo");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(FRAMES, TimerFunction, 1);
	glutMainLoop();
	return 0;
}
