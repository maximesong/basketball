#ifndef _VECTOR_H_
#define _VECTOR_H_

class Vector3d {
public:
	Vector3d(float *v);
	Vector3d(double v1 = 0.0, double v2 = 0.0, double v3 = 0.0);
	float &operator[](int i);
	float vec[3];
};

class Vector4d {
public:
	Vector4d(float *v);
	Vector4d(double v1 = 0.0, double v2 = 0.0, double v3 = 0.0, double v4 = 0.0);
	float vec[4];
	float &operator[](int i);
};

#endif /* _VECTOR_H_ */

