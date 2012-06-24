#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Vector.h"

class Light {
public:
	Light(Vector4d pos, Vector4d ambient, Vector4d diffuse,
	      Vector4d specular);
	Light(Vector4d ambient, Vector4d diffuse,
	      Vector4d specular);
	Light();
	Vector4d ambient;
	Vector4d diffuse;
	Vector4d specular;
	Vector4d pos;
};

#endif /* _LIGHT_H_ */
