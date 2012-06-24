#include "Vector.h"

#include <string.h>

Vector3d::Vector3d(float *v)
{
	memcpy(v, vec, 3 * sizeof(float));
}

Vector3d::Vector3d(double v1, double v2, double v3)
{
	vec[0] = v1;
	vec[1] = v2;
	vec[2] = v3;
}

float &Vector3d::operator[](int i)
{
	if (i >= 0 && i <= 2)
		return vec[i];
	else 
		return vec[0];
}

Vector4d::Vector4d(double v1, double v2, double v3, double v4)
{
	vec[0] = v1;
	vec[1] = v2;
	vec[2] = v3;
	vec[3] = v4;
}

float &Vector4d::operator[](int i)
{
	if (i >= 0 && i <= 3)
		return vec[i];
	else 
		return vec[0];
}


Vector4d::Vector4d(float *v)
{
	memcpy(v, vec, 4 * sizeof(float));
}
