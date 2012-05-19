#include "BasketballWidget.h"

void BasketballWidget::initializeGL()
{
#ifdef __linux__
//	Phonon::MediaSource ms("music/hit.wav");
//	cout << ms.discType() << endl;
	player = Phonon::createPlayer(Phonon::MusicCategory, 
				      Phonon::MediaSource("music/hit.wav"));
	player->play();
#endif
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
    
	// Setup light parameters
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);


	// Calculate shadow matrix

	M3DVector3f vPoints[3] = {{ 0.0f, 0.3f, 0.0f },
				  { 10.0f, 0.3f, 0.0f },
				  { 5.0f, 0.4f, -5.0f }};

	M3DVector4f pPlane;
	m3dGetPlaneEquation(pPlane, vPoints[0], vPoints[1], vPoints[2]);
	m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, fLightPos);

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
		GLbyte *pBytes;
		GLint iWidth, iHeight, iComponents;
		GLenum eFormat;
        
		glBindTexture(GL_TEXTURE_2D, textures[i]);
        
		// Load this texture map
		pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, 
				    &iComponents, &eFormat);
		gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, 
				  eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
        
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void BasketballWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);

	if (eyey > 2) {
		eyey -= 0.05;
	}
	
	gluLookAt(eyex + ball.x, eyey, eyez + ball.z, 
		  0.9 * ball.x, -eyey, 0.9 * ball.z ,0, 1, 0);

	glTranslatef(0, 0, 10);

//	glDepthMask(GL_TRUE);

	drawGround(0, -0.3, 0, 28, 0.3, 15);

	glPushMatrix();
	glTranslatef(ball.x, ball.y, ball.z);

	if (ball.is_hit) {
		glScalef(1, 0.95, 1);
		QSound::play("music/hit.wav");
#ifdef __linux__
		player->play();
#endif
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
	gltDrawSphere(ball.r, 40, 20);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPopMatrix();
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

	glRotatef(ball.rotation, ball.rx, ball.ry, ball.rz);

//	cout << ball.rotation << "\t" << ball.x << "\t" << ball.y << "\t" << ball.z << endl;

	glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
	gltDrawSphere(ball.r, 40, 20);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
   
	glPopMatrix();
}

void BasketballWidget::drawGround(GLfloat x, GLfloat y, GLfloat z,
				  GLfloat w, GLfloat h, GLfloat d)
{
	glBindTexture(GL_TEXTURE_2D, textures[GROUND_TEXTURE]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
	glTexCoord2f(0.0, 0.0);
	glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x-w, y+h, z+d);

	glTexCoord2f(1.0, 0.0);
	glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x+w, y+h, z+d);

	glTexCoord2f(1.0, 1.0);
	glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x+w, y+h, z-d);

	glTexCoord2f(0.0, 1.0);
	glNormal3f(0.0f, 1.0f, 0.0f);
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

void BasketballWidget::timeFunc(int)
{
	ball = calculateXyFallingBall(ball, env, 1.0 / FRAMES);
	updateGL();
}
