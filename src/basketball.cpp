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

GLboolean ambientShadowAvailable = GL_FALSE;
GLboolean npotTexturesAvailable = GL_FALSE;
GLboolean controlCamera = GL_TRUE;      // xyz keys will control lightpos
GLboolean noShadows = GL_FALSE;         // normal lighting
GLboolean showShadowMap = GL_FALSE;     // show the shadowmap texture
GLdouble cameraZoom = 0.3;

GLfloat factor = 4.0f;                  // for polygon offset

GLuint shadowTextureID;

GLint maxTexSize;                       // maximum allowed size for 1D/2D texture
GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f};
GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f};
GLfloat noLight[]      = { 0.0f, 0.0f, 0.0f, 1.0f};


GLint windowWidth = 800;               // window size
GLint windowHeight = 600;
GLint shadowWidth = 800;               // set based on window size
GLint shadowHeight = 600;
M3DMatrix44f textureMatrix;

int light0_on;

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
void init_shadow();
void RegenerateShadowMap();
void SetupRC();
void set_lights();

void init()
{
#ifdef __linux__
	player = Phonon::createPlayer(Phonon::MusicCategory, 
				      Phonon::MediaSource(
					      "music/hit.wav"));
#endif
	board  = new Board(MEDIUM);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

//	SetupRC();
	init_lights();
	init_textures();
	init_ball();
	init_shadow();
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
	light0_on = 1;
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

 	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, 20);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fSpotDirection);
	glLightf( GL_LIGHT1 , GL_SPOT_EXPONENT , 2); 
}

void init_lights()
{
	glEnable(GL_LIGHTING);        //启动光照

	init_light0();
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
	m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, fSpotPos);
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

	eyex=2;
	eyey=2;
	eyez=15;
	eyey += ball.y;
}



void drawGround(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat d)
{
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, textures[AUDIENCE_TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, textures[GROUND_TEXTURE]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glEnable(GL_NORMALIZE);
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
	    glTexCoord2f(i / 300.0f, j / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * i / 300.0f,
			   0.0,
			   yFrom + (yTo - yFrom) * j / 200.0f);
	    glTexCoord2f( i / 300.0f, (j + 1) / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * i / 300.0f,
	               0.0,
	               yFrom + (yTo - yFrom) * (j + 1) / 200.0f);
	    glTexCoord2f((i + 1) / 300.0f, (j + 1) / 200.0f);
	    glVertex3f(xFrom + (xTo - xFrom) * (i + 1) / 300.0f,
	               0.0,
	               yFrom + (yTo - yFrom) * (j + 1) / 200.0f);
	    glTexCoord2f((i + 1) / 300.0f, j / 200.0f);
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

	glBindTexture(GL_TEXTURE_2D, textures[BALL_TEXTURE]);
	init_shadow();
        // Draw shadows first
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);

	glMultMatrixf(mShadowMatrix);
	set_lights();
 	glTranslatef(ball.x, ball.y, ball.z);
        glColor4f(0.00f, 0.00f, 0.00f, .6f);  // Shadow color
	gltDrawSphere(ball.r, 40, 20);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glPopMatrix();

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

	glPushMatrix();
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
 	glTranslatef(ball.x, ball.y, ball.z);
	glRotatef(ball.rotation, ball.rx, ball.ry, ball.rz);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
	gltDrawSphere(ball.r, 40, 20);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);

//	cout << ball.rotation << "\t" << ball.x << 
//		"\t" << ball.y << "\t" << ball.z << endl;

 	glPopMatrix();
}

void draw_ball_shadow()
{
	glPushMatrix();

 	glTranslatef(ball.x, ball.y, ball.z);
	glBindTexture(GL_TEXTURE_2D, textures[BALL_TEXTURE]);
	init_shadow();
        // Draw shadows first
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);

	glMultMatrixf(mShadowMatrix);
        glColor4f(0.00f, 0.00f, 0.00f, .6f);  // Shadow color
	gltDrawSphere(ball.r, 40, 20);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glPopMatrix();

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
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
 	glTranslatef(-22, 2, -0.5);
 	glRotatef(90, 0, 1, 0);
 	board->displayBoard();
 	glPopMatrix();

 	glPushMatrix();
 	glTranslatef(22, 2, -0.5);
 	glRotatef(-90, 0, 1, 0);
 	board->displayBoard();
 	glPopMatrix();
}

void set_lights()
{
	/* Light 0 */
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);

	/* Light 1 */
	fSpotPos[0] = ball.x;
	fSpotPos[2] = ball.z;
 	glLightfv(GL_LIGHT1, GL_POSITION, fSpotPos);
	fSpotDirection[0] = fSpotPos[0] - ball.x;
	fSpotDirection[2] = fSpotPos[2] - ball.z;
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, fSpotDirection);
	glLightf( GL_LIGHT1 , GL_SPOT_EXPONENT , 5); 
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
		GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 	if (eyey > 2) {
 		eyey -= 0.05;
 	}
	glPushMatrix();
 	gluLookAt(eyex + ball.x, eyey, eyez + ball.z, 
		  0.9 * ball.x, -eyey, 0.9 * ball.z ,0, 1, 0);
//	gluLookAt(eyex, eyey, eyez, 
//		  0, 0, 0,
//		  0, 0, -1);

	set_lights();

	draw_boards();
 	drawGround(0, -0.3, 0, 28, 0.3, 15);
 	drawGym(0, 2.5, 0, 28, 2.5, 15);
 	if (ball.is_hit)
		play_sound();
	draw_ball();
	glPopMatrix();

	// glPushMatrix();
	// draw_ball_shadow();
	// glPopMatrix();
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
	else if (key == 'r')
		init_ball();
	else if (key == 'b')
		if (light0_on) {
			light0_on = 0;
			glDisable(GL_LIGHT0);
		} else {
			light0_on = 1;
			glEnable(GL_LIGHT0);
		}
}

// Called to regenerate the shadow map
void RegenerateShadowMap()
{
	GLfloat lightToSceneDistance, nearPlane, fieldOfView;
	GLfloat lightModelview[16], lightProjection[16];
	GLfloat sceneBoundingRadius = 30.0f; // based on objects in scene

	// Save the depth precision for where it's useful
	lightToSceneDistance = sqrt(fSpotPos[0] * fSpotPos[0] + 
				    fSpotPos[1] * fSpotPos[1] + 
				    fSpotPos[2] * fSpotPos[2]);
	nearPlane = lightToSceneDistance - sceneBoundingRadius;
	// Keep the scene filling the depth texture
	fieldOfView = 
		(GLfloat)m3dRadToDeg(2.0f * 
				     atan(sceneBoundingRadius /
					  lightToSceneDistance));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, 1.0f, nearPlane, 
		       nearPlane + (2.0f * sceneBoundingRadius));
	glGetFloatv(GL_PROJECTION_MATRIX, lightProjection);
	// Switch to light's point of view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(fSpotPos[0], fSpotPos[1], fSpotPos[2], 
		  ball.x, 0.0f, ball.z, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightModelview);
	glViewport(0, 0, shadowWidth, shadowHeight);

	// Clear the depth buffer only
	glClear(GL_DEPTH_BUFFER_BIT);

	// All we care about here is resulting depth values
	glShadeModel(GL_FLAT);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glColorMask(0, 0, 0, 0);

	// Overcome imprecision
	glEnable(GL_POLYGON_OFFSET_FILL);

	// Draw objects in the scene except base plane
	// which never shadows anything
	draw_ball();
	draw_boards();

	// Copy depth values into depth texture
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
			 0, 0, shadowWidth, shadowHeight, 0);

	// Restore normal drawing state
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glColorMask(1, 1, 1, 1);
	glDisable(GL_POLYGON_OFFSET_FILL);

	// Set up texture matrix for shadow map projection,
	// which will be rolled into the eye linear
	// texture coordinate generation plane equations
	M3DMatrix44f tempMatrix;
	m3dLoadIdentity44(tempMatrix);
	m3dTranslateMatrix44(tempMatrix, 0.5f, 0.5f, 0.5f);
	m3dScaleMatrix44(tempMatrix, 0.5f, 0.5f, 0.5f);
	m3dMatrixMultiply44(textureMatrix, tempMatrix, lightProjection);
	m3dMatrixMultiply44(tempMatrix, textureMatrix, lightModelview);
	// transpose to get the s, t, r, and q rows for plane equations
	m3dTransposeMatrix44(textureMatrix, tempMatrix);
}


void SetupRC()
{
	fprintf(stdout, "Shadow Mapping Demo\n\n");

	// Make sure required functionality is available!
	if (!GLEE_VERSION_1_4 && !GLEE_ARB_shadow)
	{
		fprintf(stderr, "Neither OpenGL 1.4 nor GL_ARB_shadow"
                        " extension is available!\n");
		exit(0);
	}

	// Check for optional extensions
	if (GLEE_ARB_shadow_ambient)
	{
		ambientShadowAvailable = GL_TRUE;
	}
	else
	{
		fprintf(stderr, "GL_ARB_shadow_ambient extension not available!\n");
		fprintf(stderr, "Extra ambient rendering pass will be required.\n\n");
	}

	if (GLEE_VERSION_2_0 || GLEE_ARB_texture_non_power_of_two)
	{
		npotTexturesAvailable = GL_TRUE;
	}
	else
	{
		fprintf(stderr, "Neither OpenGL 2.0 nor GL_ARB_texture_non_power_of_two extension\n");
		fprintf(stderr, "is available!  Shadow map will be lower resolution (lower quality).\n\n");
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

	fprintf(stdout, "Controls:\n");
	fprintf(stdout, "\tRight-click for menu\n\n");
	fprintf(stdout, "\tx/X\t\tMove +/- in x direction\n");
	fprintf(stdout, "\ty/Y\t\tMove +/- in y direction\n");
	fprintf(stdout, "\tz/Z\t\tMove +/- in z direction\n\n");
	fprintf(stdout, "\tf/F\t\tChange polygon offset factor +/-\n\n");
	fprintf(stdout, "\tq\t\tExit demo\n\n");
    
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	// Hidden surface removal
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glPolygonOffset(factor, 0.0f);

	// Set up some lighting state that never changes
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);

	// Set up some texture state that never changes
	glGenTextures(1, &shadowTextureID);
	glBindTexture(GL_TEXTURE_2D, shadowTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	if (ambientShadowAvailable)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 
				0.5f);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	RegenerateShadowMap();
}

void DrawModels(GLboolean)
{
	/* empty */
}

void RenderScene(void)
{
	// Track camera angle
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble ar = (GLdouble)windowWidth / (GLdouble)windowHeight;
	glFrustum(-ar * cameraZoom, ar * cameraZoom, -cameraZoom,
		  cameraZoom, 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, 
		  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glViewport(0, 0, windowWidth, windowHeight);
    
	// Track light position
//	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!ambientShadowAvailable)
	{
		GLfloat lowAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
		GLfloat lowDiffuse[4] = {0.35f, 0.35f, 0.35f, 1.0f};

		// Because there is no support for an "ambient"
		// shadow compare fail value, we'll have to
		// draw an ambient pass first...
		glLightfv(GL_LIGHT0, GL_AMBIENT, lowAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lowDiffuse);

		// Draw objects in the scene, including base plane
		DrawModels(GL_TRUE);

		// Enable alpha test so that shadowed fragments are discarded
		glAlphaFunc(GL_GREATER, 0.9f);
		glEnable(GL_ALPHA_TEST);
	}

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

        // Set up shadow comparison
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, 
                        GL_COMPARE_R_TO_TEXTURE);

        // Set up the eye plane for projecting the shadow map on the scene
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);
        glTexGenfv(GL_S, GL_EYE_PLANE, &textureMatrix[0]);
        glTexGenfv(GL_T, GL_EYE_PLANE, &textureMatrix[4]);
        glTexGenfv(GL_R, GL_EYE_PLANE, &textureMatrix[8]);
        glTexGenfv(GL_Q, GL_EYE_PLANE, &textureMatrix[12]);

        // Draw objects in the scene, including base plane
        DrawModels(GL_TRUE);

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);   
	glDisable(GL_TEXTURE_GEN_R); 
	glDisable(GL_TEXTURE_GEN_Q);  

	if (glGetError() != GL_NO_ERROR)
		fprintf(stderr, "GL Error!\n");

	// Flush drawing commands
	glutSwapBuffers();
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
//	glutDisplayFunc(RenderScene);
	glutDisplayFunc(display);
	glutKeyboardFunc(processNormalKeys);
	glutReshapeFunc(reshape);
	glutTimerFunc(FRAMES, TimerFunction, 1);
	glutMainLoop();
	return 0;
}
