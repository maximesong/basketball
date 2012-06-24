#include "World.h"

#include <cmath>

#define PI 3.141592653589

World::World()
{
}

void World::go(double t)
{
	BallModel next_ball = ball;
	next_ball.is_hit = 0;
	
	double v = sqrt(ball.v[0] * ball.v[0]  + ball.v[1] * ball.v[1] + ball.v[2] * ball.v[2]);

	double air_common_coefficient = -0.5 * air_density * 
		ball.drag_coefficient * PI * ball.radius * ball.radius * v;
	double f_air_x = air_common_coefficient * ball.v[0];
	double f_air_y = air_common_coefficient * ball.v[1];
	double f_air_z = air_common_coefficient * ball.v[2];


	/*
	 * v[0], v[1], v[2]
	 * ra[0], ra[1], ra[2]
	 */
	double mx = ball.v[1] * ball.ra[2] - ball.v[2] * ball.ra[1];
	double my = ball.v[2] * ball.ra[0] - ball.v[0] * ball.ra[2];
	double mz = ball.v[0] * ball.ra[1] - ball.v[1] * ball.ra[0];
	double m = sqrt(mx * mx + my * my + mz * mz);
	double m_common_coefficient = 0.5 * air_density *
		ball.m_coefficient * PI * ball.radius * ball.radius * v * v;
	
	double f_m_x;
	double f_m_y;
	double f_m_z;
	if (m != 0) {
		f_m_x = m_common_coefficient * mx / m;
		f_m_y = m_common_coefficient * my / m;
		f_m_z = m_common_coefficient * mz / m;
	} else {
		f_m_x =  f_m_y = f_m_z = 0;
	}

	double f_g = gravity * ball.weight;

	double fx = f_air_x + f_m_x;
	double fy = f_air_y + f_m_y + f_g;
	double fz = f_air_z + f_m_z;
	
	double f_rolling_x = 0;
	double f_rolling_z = 0;

	if (ball.is_almost_rolling) {
		double f_rolling = f_g * ball.rolling_coefficient;
		double hv = sqrt(ball.v[0] * ball.v[0] + ball.v[2] * ball.v[2]);
		if (ball.is_rolling) {
			f_rolling *= 2;
		}
		if (hv != 0) {
			f_rolling_x = ball.v[0] / hv * f_rolling;
			f_rolling_z = ball.v[2] / hv * f_rolling;
		} 
	}

	double ax = fx / ball.weight;
	double ay = fy / ball.weight;
	double az = fz / ball.weight;

	next_ball.v[0] = ball.v[0] + ax * t;
	next_ball.v[1] = ball.v[1] + ay * t;
	next_ball.v[2] = ball.v[2] + az * t;

	double vx_with_rolling = next_ball.v[0] +  f_rolling_x / ball.weight * t;
	double vz_with_rolling = next_ball.v[2] + f_rolling_z / ball.weight * t;
	if (vx_with_rolling * next_ball.v[0] <= 0)
		next_ball.v[0] = 0;
	else
		next_ball.v[0] = vx_with_rolling;

	if (vz_with_rolling * next_ball.v[2] <= 0)
		next_ball.v[2] = 0;
	else
		next_ball.v[2] = vz_with_rolling;


	next_ball.pos[0] = next_ball.v[0] * t + ball.pos[0];
	next_ball.pos[1] = next_ball.v[1] * t + ball.pos[1];
	next_ball.pos[2] = next_ball.v[2] * t + ball.pos[2];

	if ((next_ball.pos[1] - next_ball.radius) <= 0) { /* hit the ground */

		next_ball.is_hit = 1;

		next_ball.pos[1] = next_ball.radius;
		
		next_ball.v[1] = -next_ball.restitution_coefficient *
			next_ball.v[1];

		if (next_ball.v[1] < 5 * gravity) {
			next_ball.v[1] *= next_ball.restitution_coefficient;
		}
		if (next_ball.v[1] + 2 * t * gravity < 0) {
			next_ball.is_almost_rolling = 1;
			next_ball.pos[1] = next_ball.v[1] * t + next_ball.pos[1];
		}
	}

	if (next_ball.is_almost_rolling) {
		next_ball.is_hit = 0;

		if (next_ball.pos[1] - next_ball.radius < 0.01) {
			next_ball.pos[1] = next_ball.radius;
			next_ball.is_rolling = 1;
		}
		else {
			next_ball.pos[1] = next_ball.pos[1] + 0.9 * (next_ball.radius - next_ball.pos[1]);
		}
		next_ball.v[1] = 0;
	}

	next_ball.ra[0] = next_ball.v[2];
	next_ball.ra[2] = next_ball.v[0];
	next_ball.ra[1] = 0;
	double r = sqrt(next_ball.ra[0] * next_ball.ra[0] + next_ball.ra[2] * next_ball.ra[2]);
	if (r != 0) {
		next_ball.ra[0] /= r;
		next_ball.ra[2] /= r;
	}
	else {
		next_ball.ra[0] = 0;
		next_ball.ra[2] = 0;
	}
	next_ball.w = sqrt(next_ball.v[0] * next_ball.v[0] + 
			   next_ball.v[2] * next_ball.v[2]) / next_ball.radius;

	next_ball.rotation += next_ball.w / (2 * PI) * t * 360;

	ball = next_ball;	
}
