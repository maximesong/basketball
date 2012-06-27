#include "basketball.h"

#include <iostream>
using namespace std;

#include <QTimer>

#define PIECES

#include "modeling.h"
#include "Board.h"

#define FRAMES 33

GLfloat eyex=2,eyey=2,eyez=15;

GLfloat fLightPos[4]   = { 0.0f, 18.0f, 0.0f, 1.0f };  // Point source
GLfloat fSpotPos[4] = {0.0f, 8.0f, 0.0f, 1.0f }; // Point source
GLfloat fSpotDirection[] = {0.0f, -1.0f, 0.0f}; // Point source
GLfloat fAllPos[] = {0.0f, 0.0f, 0.0f, 0.0f}; // Point source

GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fVeryLowLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat fStrongLight[] = {0.8f, 0.8f, 0.8f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

Board *board;

#define GROUND_TEXTURE  0
#define BALL_TEXTURE   1
#define AUDIENCE_TEXTURE 2
#define NUM_TEXTURES    3

GLuint  textures[NUM_TEXTURES];
const char *szTextureFiles[] = { "texture/houston.tga", 
				 "texture/basketball.tga", 
				 "texture/audience.tga"};

M3DMatrix44f mShadowMatrix;

Ball ball;
Env env;


GLfloat light_position[4] = {0,0,0,1};        //光位置
GLfloat light_diffuse[4] = {1,0,1,1};        //光的漫反色
GLfloat light_direction[4] = {-1, -2, -1, 1};    //光源聚光灯方向
GLfloat light_change[3] = {3,3,-1};            //光源移动位置。

#ifdef __linux__
Phonon::MediaObject *player;
#endif

void init_textures();
void init_lights();
void init_ball();
void init()
{
#ifdef __linux__
	player = Phonon::createPlayer(Phonon::MusicCategory, 
				      Phonon::MediaSource(
					      "music/hit.wav"));
#endif
	board  = new Board(MEDIUM);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f );


	init_lights();
	init_textures();
	init_ball();
}

void init_textures()
{
	// Set up texture maps
	glEnable(GL_TEXTURE_2D);
        glGenTextures(NUM_TEXTURES, textures);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
//	Load the texture objects
	for(int i = 0; i < NUM_TEXTURES; i++) {
		GLbyte *pBytes;
		GLint iWidth, iHeight, iComponents;
		GLenum eFormat;
        
		glBindTexture(GL_TEXTURE_2D, textures[i]);
        
		// Load this texture map
		pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, 
				    &iComponents, &eFormat);
		gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, 
				  iHeight, eFormat, GL_UNSIGNED_BYTE, 
				  pBytes);
		free(pBytes);
        
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
}

void init_light0()
{
	glEnable(GL_LIGHT0);
 	glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
 	glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
 	glLightfv(GL_LIGHT0, GL_SPECULAR, fNoLight);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
//    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 50.0); //设置聚光的范围为45度。
//    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,2.0);    //设置聚光灯的聚光强度。
    
}

void init_light1()
{
	glEnable(GL_LIGHT1);

 	glLightfv(GL_LIGHT1, GL_AMBIENT, fLowLight);
 	glLightfv(GL_LIGHT1, GL_DIFFUSE, fBrightLight);
 	glLightfv(GL_LIGHT1, GL_SPECULAR, fBrightLight);
 	glLightfv(GL_LIGHT1, GL_POSITION, fSpotPos);

// 	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, 40);
//	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fSpotDirection);
//	glLightf( GL_LIGHT1 , GL_SPOT_EXPONENT , 2); 
}

void init_lights()
{
	glEnable(GL_LIGHTING);        //启动光照

//	init_light0();
	init_light1();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fStrongLight);
	glMaterialfv(GL_FRONT, GL_AMBIENT, fStrongLight);
	glMaterialf(GL_FRONT, GL_SHININESS, 64);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, 
		      GL_SEPARATE_SPECULAR_COLOR);    

	glCullFace(GL_BACK);        //剔除背面
	glEnable(GL_CULL_FACE);        //启动剔除
	glDepthFunc(GL_LEQUAL);        //深度检测为小于等于
	glEnable(GL_DEPTH_TEST);    //启动深度检测
	glFrontFace(GL_CCW);        //定义逆时针为正面
}

void init_shadow()
{
	glStencilOp(GL_INCR, GL_INCR, GL_INCR);
	glClearStencil(0);
	glStencilFunc(GL_EQUAL, 0x0, 0x01);

	M3DVector3f vPoints[3] = {{ 0.0f, 0.3f, 0.0f },
				  { 10.0f, 0.3f, 0.0f },
				  { 5.0f, 0.4f, -5.0f }};

	M3DVector4f pPlane;
	m3dGetPlaneEquation(pPlane, vPoints[0], vPoints[1], vPoints[2]);
	m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, fLightPos);
}

void init_ball()
{
	/* The init of the ball */
	ball.x = -10;
	ball.y = 5;
	ball.z = -3;
	ball.r = 0.12;
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



void drawGround(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat d)
{
	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, textures[AUDIENCE_TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, textures[GROUND_TEXTURE]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glEnable(GL_NORMALIZE);
	glBegin(GL_QUADS);
        // //front
        // glVertex3f(x-w, y+h, z+d);
        // glVertex3f(x-w, y-h, z+d);
        // glVertex3f(x+w, y-h, z+d);
        // glVertex3f(x+w, y+h, z+d);

        // //bottom
        // glVertex3f(x-w, y-h, z+d);
        // glVertex3f(x+w, y-h, z+d);
        // glVertex3f(x+w, y-h, z-d);
        // glVertex3f(x-w, y-h, z-d);

        // //back
        // glVertex3f(x+w, y+h, z-d);
        // glVertex3f(x+w, y-h, z-d);
        // glVertex3f(x-w, y-h, z-d);
        // glVertex3f(x-w, y+h, z-d);
    
        //top, all point up
	// glNormal3f(0.0f, 1.0f, 0.0f);
	// double x_from = x - w;
	// double x_diff = 2 * w;
	// double z_from = z - d;
	// double z_diff = 2 * z;
	// double y0 = y + h;
	// double dx = 0.01 * x_diff;
	// double dz = 0.01 * z_diff;

	// for (double ux = 0; ux < 1; ux += 0.01) {
	// 	double x0 = x_from + x_diff * ux;
	// 	for (double uz = 0; uz < 1; uz += 0.01) {
	// 		double z0 = z_from + z_from * uz;
	// 		cout << z0 << "\t" << x0 << endl;
	// 		glTexCoord2f(ux, uz);
	// 		glVertex3f(x0, y0, z0 + dz);
	// 		glTexCoord2f(ux + 0.01, uz);
	// 		glVertex3f(x0 + dx, y0, z0 + dz);
	// 		glTexCoord2f(ux + 0.01, uz + 0.01);
	// 		glVertex3f(x0 + dx, y0, z0);
	// 		glTexCoord2f(ux, uz + 0.01);
	// 		glVertex3f(x0, y0, z0);
	// 	}
	// }
#ifdef PIECES
	float xFrom =  x - w;
	float xTo = x + w;
	float yFrom = z - d;
	float yTo =  z + d;
	glNormal3f(0.0, 1.0, 0.0);
	for (int i = 0;i < 300;++i)
	  for (int j = 0;j < 200;++j)
	  {
//	    glTexCoord2f(i / 300.0f, j / 200.0f);
	    glTexCoord2f(xFrom + (xTo - xFrom) * i / 300.0f,
			 yFrom + (yTo - yFrom) * j / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * i / 300.0f,
			   0.0,
			   yFrom + (yTo - yFrom) * j / 200.0f);
//	    glTexCoord2f( i / 300.0f, (j + 1) / 200.0f);
	    glTexCoord2f(xFrom + (xTo - xFrom) * i / 300.0f,
	               yFrom + (yTo - yFrom) * (j + 1) / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * i / 300.0f,
	               0.0,
	               yFrom + (yTo - yFrom) * (j + 1) / 200.0f);
//	    glTexCoord2f((i + 1) / 300.0f, (j + 1) / 200.0f);
	    glTexCoord2f(xFrom + (xTo - xFrom) * (i + 1) / 300.0f,
			 yFrom + (yTo - yFrom) * (j + 1) / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * (i + 1) / 300.0f,
	               0.0,
	               yFrom + (yTo - yFrom) * (j + 1) / 200.0f);
//	    glTexCoord2f((i + 1) / 300.0f, j / 200.0f);
	    glTexCoord2f(xFrom + (xTo - xFrom) * (i + 1) / 300.0f,
			 yFrom + (yTo - yFrom) * j / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * (i + 1) / 300.0f,
	               0.0,
	               yFrom + (yTo - yFrom) * j / 200.0f);
	  }
#else
//	Front  whole
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
        glVertex3f(x-w, y+h, z+d);
	glTexCoord2f(1.0, 0.0);
        glVertex3f(x+w, y+h, z+d);
	glTexCoord2f(1.0, 1.0);
        glVertex3f(x+w, y+h, z-d);
	glTexCoord2f(0.0, 1.0);
        glVertex3f(x-w, y+h, z-d);
#endif
        // //left
        // glVertex3f(x-w, y+h, z+d);
        // glVertex3f(x-w, y-h, z+d);
        // glVertex3f(x-w, y-h, z-d);
        // glVertex3f(x-w, y+h, z-d);

        // //right
        // glVertex3f(x+w, y+h, z+d);
        // glVertex3f(x+w, y-h, z+d);
        // glVertex3f(x+w, y-h, z-d);
        // glVertex3f(x+w, y+h, z-d);
 
        glEnd();
	glDisable(GL_TEXTURE_2D);
}


void drawGym(GLfloat x, GLfloat y, GLfloat z, 
	     GLfloat w, GLfloat h, GLfloat d)
{
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, textures[GROUND_TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, textures[AUDIENCE_TEXTURE]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_QUADS);

        //front

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
        glVertex3f(x-w, y+h, z-d);
	glTexCoord2f(1.0, 0.0);
        glVertex3f(x-w, y-h, z-d);
	glTexCoord2f(0.0, 0.0);
        glVertex3f(x+w, y-h, z-d);
	glTexCoord2f(0.0, 1.0);
        glVertex3f(x+w, y+h, z-d);

        //back
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0, 1.0);
        glVertex3f(x+w, y+h, z+d);
	glTexCoord2f(0.0, 0.0);   
        glVertex3f(x+w, y-h, z+d);
	glTexCoord2f(1.0, 0.0); 
        glVertex3f(x-w, y-h, z+d);
	glTexCoord2f(1.0, 1.0);
        glVertex3f(x-w, y+h, z+d);
/*
//top, all point up
glNormal3f(0.0f, -1.0f, 0.0f);
glTexCoord2f(0.0, 0.0);
glVertex3f(x-w, y+h, z+d);
glTexCoord2f(1.0, 0.0);
glVertex3f(x+w, y+h, z+d);
glTexCoord2f(1.0, 1.0);
glVertex3f(x+w, y+h, z-d);
glTexCoord2f(0.0, 1.0);
glVertex3f(x-w, y+h, z-d);

//bottom
glTexCoord2f(0.0, 0.0);
glVertex3f(x-w, y-h, z+d);
glTexCoord2f(1.0, 0.0);
glVertex3f(x+w, y-h, z+d);
glTexCoord2f(1.0, 1.0);
glVertex3f(x+w, y-h, z-d);
glTexCoord2f(0.0, 1.0);
glVertex3f(x-w, y-h, z-d);
*/

        //left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 1.0);
        glVertex3f(x-w, y+h, z+d);
	glTexCoord2f(0.0, 0.0);
        glVertex3f(x-w, y-h, z+d);
	glTexCoord2f(1.0, 0.0);
        glVertex3f(x-w, y-h, z-d);
	glTexCoord2f(1.0, 1.0);
        glVertex3f(x-w, y+h, z-d);

	// right
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 1.0);
        glVertex3f(x+w, y+h, z-d);
	glTexCoord2f(0.0, 0.0);
        glVertex3f(x+w, y-h, z-d);
	glTexCoord2f(1.0, 0.0);
        glVertex3f(x+w, y-h, z+d);
	glTexCoord2f(1.0, 1.0);
        glVertex3f(x+w, y+h, z+d);
 
        glEnd();
	glDisable(GL_TEXTURE_2D);
}


void spot_example()
{
	glTranslated(0, -2,-5);    //平移0,0,-5向Z负方向平移5个单位

	glPushMatrix();        //压缩矩阵，设定光源的位置
	glDisable(GL_LIGHTING);    
	glRotatef(10, 0, 1, 0);    //光源的旋转
	glTranslatef(light_change[0],light_change[1],light_change[2]);
	glTranslatef(light_position[0], light_position[1],
		     light_position[2]); 
	glutSolidSphere(0.1, 4,4);//利用库函数绘制一个半径为1的球体。表示光源在这里
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glPopMatrix();

	light_direction[0] = -light_change[0];
	light_direction[1] = -light_change[1];
	light_direction[2] = -light_change[2];
	light_change[0] -= 0.1;
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);


	glPushMatrix();            //要入矩阵，绘制地面
	glColor3f(0.8, 0, 0.8);
	glBegin(GL_QUADS);
	glVertex3f(-3,0,3);
	glVertex3f(3,0,3);
	glVertex3f(3,0,-3);
	glVertex3f(-3,0,-3);
	glEnd();
	glPopMatrix();            //地面绘制完毕

	glPushMatrix();            //压入变换矩阵那
	glutSolidSphere(1, 30,30);//利用库函数绘制一个半径为1的球体。
	glPopMatrix();            //弹出矩阵。

	glPushMatrix();
	drawGround(0, 0, 0, 8, 0.3, 8);
	drawGym(0, 2.5, 0, 28, 2.5, 15);
	glPopMatrix();
}

void draw_ball()
{
 	glPushMatrix();

 	glTranslatef(ball.x, ball.y, ball.z);
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
	gltDrawSphere(ball.r, 40, 20);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPopMatrix();

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
	glRotatef(ball.rotation, ball.rx, ball.ry, ball.rz);

	glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
	gltDrawSphere(ball.r, 40, 20);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);

//	cout << ball.rotation << "\t" << ball.x << 
//		"\t" << ball.y << "\t" << ball.z << endl;

 	glPopMatrix();
}

void play_sound()
{
	glScalef(1, 0.95, 1);
	QSound::play("music/hit.wav");
#ifdef __linux__
	player->play();
#endif
}

void draw_boards()
{
 	glPushMatrix();
 	glTranslatef(-22, 2, 10.3);
 	glRotatef(90, 0, 1, 0);
 	board->displayBoard();
 	glPopMatrix();

 	glPushMatrix();
 	glTranslatef(22, 2, 10.3);
 	glRotatef(-90, 0, 1, 0);
 	board->displayBoard();
 	glPopMatrix();
}

void set_lights()
{
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);

// //	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
// //	glLightfv(GL_LIGHT0, GL_POSITION, fSpotPos);
// //	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF, 40);
// //	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fSpotDirection);

// 	glLightfv(GL_LIGHT1, GL_AMBIENT, fLowLight);
// 	glLightfv(GL_LIGHT1, GL_DIFFUSE, fBrightLight);
// 	glLightfv(GL_LIGHT1, GL_SPECULAR, fLowLight);
 	glLightfv(GL_LIGHT1, GL_POSITION, fSpotPos);
 	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, 40);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fSpotDirection);
	glLightf( GL_LIGHT1 , GL_SPOT_EXPONENT , 2); 

// //	glLightfv(GL_LIGHT1, GL_POSITION, fLightPos);
// //	glLightfv(GL_LIGHT1, GL_POSITION, fSpotPos);
// //	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fSpotDirection);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
		GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

// 	if (eyey > 2) {
// 		eyey -= 0.05;
// 	}
// 	gluLookAt(eyex + ball.x, eyey, eyez + ball.z, 
//		  0.9 * ball.x, -eyey, 0.9 * ball.z ,0, 1, 0);

	gluLookAt(eyex, eyey, eyez, 
		  0, 0, 0,
		  0, 0, -1);

	set_lights();

	draw_boards();
 	drawGround(0, -0.3, 0, 28, 0.3, 15);
 	drawGym(0, 2.5, 0, 28, 2.5, 15);
 	if (ball.is_hit)
		play_sound();
	draw_ball();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w , (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h ==0 ) h = 1;
	gluPerspective(45.0f,(GLfloat) w / (GLfloat)h , 0.5f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunction(int)
{
	ball = calculateXyFallingBall(ball, env, 1.0 / FRAMES);
	glutPostRedisplay();
	glutTimerFunc(FRAMES, TimerFunction, 1);
}

void processNormalKeys(unsigned char key,int,int)
{
//	cout << key << endl;
	if(key == 'w')
		eyez += 1;
	else if (key == 's')
		eyez -= 1;
	else if(key == 'd')
		eyex += 1;
	else if (key == 'a')
		eyex -= 1;
	else if (key == 'e')
		eyey += 1;
	else if (key == 'q')
		eyey -= 1;
	else if (key == 'i')
		ball.vz += 1;
	else if (key == 'j')
		ball.vx -= 1;
	else if (key == 'l')
		ball.vx += 1;
	else if (key == 'k')
		ball.vz -= 1;
	else if (key == 'o') {
		if (ball.is_rolling) {
			ball.is_almost_rolling = 0;
			ball.is_rolling = 0;
		}
		ball.vy += 1;
	}
	else if (key == 'u')
		ball.vy -= 1;
}

int main(int argc, char *argv[])
{
#ifdef __linux__
	QApplication app(argc, argv);
#endif
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Basketball Demo");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(processNormalKeys);
	glutReshapeFunc(reshape);
	glutTimerFunc(FRAMES, TimerFunction, 1);
	glutMainLoop();
	return 0;
}
