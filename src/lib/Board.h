#pragma once

#include <GL/glut.h>
#include <GL/glu.h>
extern "C" {
#include "gltx.h"
}
#include "boardInfo.h"
#include "Vector3d.h"
#include "defines.h"
#include "ring.h"

using namespace std;

static GLuint courtTexture;
static GLuint crowdTexture;
static GLuint billboardTexture;

class Board
{
private:
	GLTXimage *courtImage;
    GLTXimage *crowdImage;
    int score[2];
	Ring* newRing;
public:
	Board(double level);
	~Board(void);
	void displayBoard();
	void drawCourt();
	void drawboard();
	void drawWall(double x, double y, int parts, bool withTexture, GLuint texture, double textureFactor, bool inverseTexture,Vector3d normal);
    void displayScore();
    void updateScore(int player1, int player2);
};
