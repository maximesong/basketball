#include "Light.h"

#include <string.h>
#include <iostream>
using namespace std;

Light::Light(Vector4d pos, Vector4d ambient, Vector4d diffuse,
      Vector4d specular)
{
	memcpy(&this->pos, &pos, sizeof(Vector3d));
	memcpy(&this->ambient, &ambient, sizeof(Vector4d));
	memcpy(&this->diffuse, &diffuse, sizeof(Vector4d));
	memcpy(&this->specular, &specular, sizeof(Vector4d));
}

Light::Light(Vector4d ambient, Vector4d diffuse,
      Vector4d specular)
{
	memcpy(&this->ambient, &ambient, sizeof(Vector4d));
	memcpy(&this->diffuse, &diffuse, sizeof(Vector4d));
	memcpy(&this->specular, &specular, sizeof(Vector4d));
	bzero(&this->pos, sizeof(Vector4d));
}

Light::Light()
{
	bzero(&this->ambient, sizeof(Vector4d));
	bzero(&this->diffuse, sizeof(Vector4d));
	bzero(&this->specular, sizeof(Vector4d));
	bzero(&this->pos, sizeof(Vector4d));
}
