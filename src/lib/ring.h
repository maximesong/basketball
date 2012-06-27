#ifndef _RING_H
#define	_RING_H
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#else
#include "glut.h"
#endif

#include "Vector3d.h"
#include "defines.h"

class Ring {
public:
    Ring(double bigR, double smallR, int faces);
    void drawRing();
    int getfaces();
    Vector3d getVertexAt(int pos);
    Vector3d getNormalAt(int pos);
private:
    Vector3d* _vertaxList;
    Vector3d* _normalList;
    int** faceLocation;
    int _faces;
};

#endif	/* _RING_H */

