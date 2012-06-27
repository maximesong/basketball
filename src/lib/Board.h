#pragma once

#ifdef __linux__

#include <GL/glut.h>
#include <GL/glu.h>
#else
#include "glu.h"
#include "glut.h"
#endif

#include "boardInfo.h"
#include "Vector3d.h"
#include "defines.h"
#include "ring.h"

using namespace std;


class Board
{
private:
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
