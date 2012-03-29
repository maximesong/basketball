#include "gltools.h"

#include <GL/glut.h>
#include <iostream>
using namespace std;

GLfloat eyex=10,eyey=10,eyez=35;

/* The coordinate of the ball */
double rix = 0;
double riy = 50;
double riz = 0;

double rx;
double ry;
double rz;

/* The init speed of the ball */
double vix = 0.1;
double viy = 1.0;

/* The current speed of the ball */
double vx;
double vy;

double g = -10;
double t = 0.1;

GLuint textures[1];

//===========================OpenGL部分=========================
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	GLbyte *pBytes;
        GLint nWidth, nHeight, nComponents;
        GLenum format;

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glGenTextures(1, textures);
        
	// Load the texture objects
        pBytes = gltLoadTGA("texture/floor.tga", &nWidth, &nHeight, &nComponents, &format);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0,
		format, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);


//    设置材质
    GLfloat mat_ambient[] = { .8, .8, .8, 1.0 };
    GLfloat mat_diffuse[] = { .8, .0, .8, 1.0 };
    GLfloat mat_specular[] = { 1.0, .0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    GLfloat light_position1[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_position2[] = { 1.0, 1.0, -1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    rx = rix;
    ry = riy;
    rz = riz;
    
    vx = vix;
    vy = viy;

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(eyex, eyey, eyez,0, 0, 0 ,0, 1, 0);
//    glScalef(1.0, 2.0, 1.0);
    glTranslatef(0, 0, 10);


    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(rx, ry, rz);
    glutSolidSphere(1, 50, 30);



   
    
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glDepthMask(GL_TRUE);

    glPopMatrix();

//    glPushMatrix();
//       glutSolidSphere(9,50,50);
//       glPopMatrix();
   
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
	if (ry < 0) {
		vy = -0.5 * vy;
		ry = 0;
	}
	if (rx > 10) {
		rx = rix;
		ry = riy;
		vx = vix;
		vy = viy;
	}

	ry += vy * t + 0.5 * g * t * t;
	rx += vx;

	vy += g * t;
	if (vx ==0 && vy < -g * t && vy > g * t && ry < 0.1) {
		rx = rix;
		ry = riy;
		vx = vix;
		vy = viy;
	}
	if (ry < 0.1) {
		vx -=  0.01 * vx * ry / 0.1;
	}
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
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
    glutTimerFunc(33, TimerFunction, 1);
    glutMainLoop();
    return 0;
}
