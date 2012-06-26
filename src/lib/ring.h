#ifndef _RING_H
#define	_RING_H
#include <cmath>
#include <GL/glut.h>
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

