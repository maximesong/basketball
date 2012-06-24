#include "BallModel.h"


BallModel::BallModel(double x, double y, double z,
		     double vx, double vy, double vz,
		     double rx, double ry, double rz,
		     double radius, double weight, double rotation, double w,
		     double restitution_coefficient,
		     double drag_coefficient,
		     double m_coefficient,
		     double rolling_coefficient,
		     bool is_almost_rolling,
		     bool is_rolling,
		     bool is_hit)
{
	this->pos[0] = x;
	this->pos[1] = y;
	this->pos[2] = z;
	this->v[0] = vx;
	this->v[1] = vy;
	this->v[2] = vz;
	this->ra[0] = rx;
	this->ra[1] = ry;
	this->ra[2] = rz;
	this->weight = weight;
	this->w = w;
	this->restitution_coefficient = restitution_coefficient;
	this->drag_coefficient = drag_coefficient;
	this->m_coefficient = m_coefficient;
	this->rolling_coefficient = rolling_coefficient;
	this->is_almost_rolling = is_almost_rolling;
	this->is_rolling = is_rolling;
	this->is_hit = is_hit;
	this->radius = radius;
	this->rotation = rotation;
}


BallModel::BallModel()
{
	/* empty */
}
