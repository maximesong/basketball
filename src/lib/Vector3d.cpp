#include "Vector3d.h"

Vector3d::Vector3d(void) {
    this->_x = 0.0;
    this->_y = 0.0;
    this->_z = 0.0;
}

Vector3d::Vector3d(Vector3d* newVector) {
    this->_x = newVector->getValueAt(0);
    this->_y = newVector->getValueAt(1);
    this->_z = newVector->getValueAt(2);
}

Vector3d::Vector3d(const Vector3d & newVector) {
    _x = newVector._x;
    _y = newVector._y;
    _z = newVector._z;
}

Vector3d::Vector3d(double newx, double newy, double newz) {
    this->_x = newx;
    this->_y = newy;
    this->_z = newz;
}

double Vector3d::getValueAt(int n) {
    if (n == 0) return this->_x;
    else if (n == 1) return this->_y;
    else if (n == 2) return this->_z;
	else return 0.0;
}

void Vector3d::setValues(Vector3d* newVector) {
    this->_x = newVector->getValueAt(0);
    this->_y = newVector->getValueAt(1);
    this->_z = newVector->getValueAt(2);
}

void Vector3d::setValueAt(int pos, double newValue) {
    if (pos == 0) this->_x = newValue;
    else if (pos == 1) this->_y = newValue;
    else if (pos == 2) this->_z = newValue;
}

void Vector3d::addValueAt(int pos, double newValue) {
    if (pos == 0) this->_x += newValue;
    else if (pos == 1) this->_y += newValue;
    else if (pos == 2) this->_z += newValue;
}

void Vector3d::add(Vector3d &v) {
	_x = v.getValueAt(0) + _x;
        _y = v.getValueAt(1) + _y;
        _z = v.getValueAt(2) + _z;
}

void Vector3d::add(Vector3d* v) {
	_x = v->getValueAt(0) + _x;
        _y = v->getValueAt(1) + _y;
        _z = v->getValueAt(2) + _z;
}

void Vector3d::subst(Vector3d &v) {
	_x = _x - v.getValueAt(0);
        _y = _y - v.getValueAt(1);
        _z = _z - v.getValueAt(2);
}

void Vector3d::subst(Vector3d* v) {
	_x = _x - v->getValueAt(0);
        _y = _y - v->getValueAt(1);
        _z = _z - v->getValueAt(2);
}

void Vector3d::changeDirection(int pos) {
    if (pos == 0) this->_x *= -1;
    else if (pos == 1) this->_y *= -1;
    else if (pos == 2) this->_z *= -1;
}

double Vector3d::distance(Vector3d& v) {
    this->subst(v);
    double dist = getLength();
    this->add(v);
    return dist;
}

double Vector3d::getLength() {
    double length = sqrt(pow(_x,2) + pow(_y,2) + pow(_z,2));
    return length;
}

void Vector3d::normalize() {
    double length = this->getLength();
    _x = _x / length;
    _y = _y / length;
    _z = _z / length;
}

double Vector3d::dot(Vector3d& v) {
    double dotProd = 0.0;
    dotProd += _x * v._x;
    dotProd += _z * v._z;
    dotProd += _y * v._y;
    return dotProd;
}
