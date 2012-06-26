#pragma once

#include <cmath>

using namespace std;

class Vector3d
{
private:
	double _x,_y,_z;
public:
	Vector3d(void);
	Vector3d(Vector3d* newVector);
    Vector3d(const Vector3d & newVector);
	Vector3d(double newx, double newy, double newz);
	//Vector3d(Vector3d & newVector);
	double getValueAt(int n);
	void setValues(Vector3d* newVector);
	void setValueAt(int pos, double newValue);
	void addValueAt(int pos, double newValue);
        void add(Vector3d &v);
        void add(Vector3d* v);
        void subst(Vector3d &v);
        void subst(Vector3d* v);
        double distance (Vector3d &v);
        void changeDirection(int pos);
        double getLength();
        void normalize();
        double dot(Vector3d &v);
};
