#ifndef _WORLD_H_
#define _WORLD_H_

#include <QVector>

#include "Light.h"
#include "BallModel.h"
#include "FlatModel.h"
#include "Camera.h"

class World {
public:
	World();

	enum WhatChanged { Changed };
	void go(double seconds);

	double gravity;
	double air_density;
	QVector<Light> lights;
	BallModel ball;
	QVector<FlatModel> flats;
	Camera camera;
};

#endif /* _WORLD_H_ */
