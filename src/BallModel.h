#ifndef _BALLMODEL_H_
#define _BALLMODEL_H_

#include "Vector.h"

class BallModel {
public:
	BallModel(double x, double y, double z,
		  double vx, double vy, double vz,
		  double rx, double ry, double rz,
		  double radius, double weight, double rotation,
		  double restitution_coefficient,
		  double drag_coefficient,
		  double m_coefficient,
		  double rolling_coefficient,
		  bool is_almost_rolling,
		  bool is_rolling,
		  bool is_hit);

	BallModel();

	double weight;

/* the radius of the ball */
	double radius;

/* The position of the ball */
	Vector3d pos;

/* the velocity of transition */
	Vector3d v;

/* the velocity of rotation */
	double m_w;

/* the rotation aix */
	Vector3d ra;

	double drag_coefficient;
	double m_coefficient;
	double restitution_coefficient;
	double rolling_coefficient;

	double rotation;

	bool is_almost_rolling;
	bool is_rolling;
	bool is_hit;
};

#endif /* _BALLMODEL_H_ */

