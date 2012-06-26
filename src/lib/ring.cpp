/* 
 * File:   ring.cpp
 * Author: Raanan
 * 
 * Created on June 17, 2009, 8:06 PM
 */

#include "ring.h"

Ring::Ring(double bigR, double smallR, int faces) {
    double uAngle = 0.0, vAngle = 0.0;
    double angleChange = (double) 360 / (double) faces;
    this->_faces = faces*faces;

    this->_vertaxList = new Vector3d [faces * faces];
    this->_normalList = new Vector3d [faces * faces];
    for (int i = 0; i < faces; i++) {
        for (int j = 0; j < faces; j++) {
            _vertaxList[j + i * faces].setValueAt(X, (bigR + smallR * cos(vAngle * PI / 180)) * cos(uAngle * PI / 180));
            _vertaxList[j + i * faces].setValueAt(Z, (bigR + smallR * cos(vAngle * PI / 180)) * sin(uAngle * PI / 180));
            _vertaxList[j + i * faces].setValueAt(Y, smallR * sin(vAngle * PI / 180));
            vAngle += angleChange;
        }
        vAngle = 0;
        uAngle += angleChange;
    }

    faceLocation = new int* [_faces];
    for (int i = 0; i < _faces; i++) faceLocation[i] = new int[4];

    for (int i = 0; i < faces; i++) {
        for (int j = 0; j < faces; j++) {
            faceLocation[j + i * faces][0] = (j + i * faces) % (_faces);
            faceLocation[j + i * faces][1] = ((j + 1) % faces + i * faces) % (_faces);
            faceLocation[j + i * faces][2] = ((j + 1) % faces + faces + i * faces) % (_faces);
            faceLocation[j + i * faces][3] = (j + faces + i * faces) % (_faces);
        }
    }

    //Normalize
    for (int i = 0; i < faces * faces; i++) {
        Vector3d vec1(_vertaxList[faceLocation[i][0]].getValueAt(X) - _vertaxList[faceLocation[i][1]].getValueAt(X),
                      _vertaxList[faceLocation[i][0]].getValueAt(Y) - _vertaxList[faceLocation[i][1]].getValueAt(Y),
                      _vertaxList[faceLocation[i][0]].getValueAt(Z) - _vertaxList[faceLocation[i][1]].getValueAt(Z));
        Vector3d vec2(_vertaxList[faceLocation[i][0]].getValueAt(X) - _vertaxList[faceLocation[i][2]].getValueAt(X),
                      _vertaxList[faceLocation[i][0]].getValueAt(Y) - _vertaxList[faceLocation[i][2]].getValueAt(Y),
                      _vertaxList[faceLocation[i][0]].getValueAt(Z) - _vertaxList[faceLocation[i][2]].getValueAt(Z));
        _normalList[i] = new Vector3d(vec1.getValueAt(Y) * vec2.getValueAt(Z) - vec1.getValueAt(Z) * vec2.getValueAt(Y),
                                      vec1.getValueAt(Z) * vec2.getValueAt(X) - vec1.getValueAt(X) * vec2.getValueAt(Z),
                                      vec1.getValueAt(X) * vec2.getValueAt(Y) - vec1.getValueAt(Y) * vec2.getValueAt(X));
        _normalList[i].normalize();
    }

}

void Ring::drawRing() {
    glPushMatrix();
    glColor3d(1.0, 0.5, 0);
    glTranslated(0.0, 1.5, 2.3);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glBegin(GL_QUADS);
    for (int i = 0; i < _faces; i++) {
        glNormal3f(_normalList[i].getValueAt(X), _normalList[i].getValueAt(Y), _normalList[i].getValueAt(Z));
        glVertex3f(_vertaxList[faceLocation[i][0]].getValueAt(X), _vertaxList[faceLocation[i][0]].getValueAt(Y), _vertaxList[faceLocation[i][0]].getValueAt(Z));
        glVertex3f(_vertaxList[faceLocation[i][1]].getValueAt(X), _vertaxList[faceLocation[i][1]].getValueAt(Y), _vertaxList[faceLocation[i][1]].getValueAt(Z));
        glVertex3f(_vertaxList[faceLocation[i][2]].getValueAt(X), _vertaxList[faceLocation[i][2]].getValueAt(Y), _vertaxList[faceLocation[i][2]].getValueAt(Z));
        glVertex3f(_vertaxList[faceLocation[i][3]].getValueAt(X), _vertaxList[faceLocation[i][3]].getValueAt(Y), _vertaxList[faceLocation[i][3]].getValueAt(Z));
    }
    glEnd();
    glColor3d(1.0, 1.0, 1.0);
    glPopMatrix();
}

int Ring::getfaces() {
    return _faces;
}

Vector3d Ring::getNormalAt(int pos) {
    return _normalList[pos];
}

Vector3d Ring::getVertexAt(int pos) {
    return _vertaxList[pos];
}
