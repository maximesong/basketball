#include "modeling.h"
#include <iostream>

using namespace std;

Ball calculateXyFallingBall(const Ball &ball, const Env &env, double t)
{
	Ball next_ball;
	next_ball.r = ball.r;
	next_ball.weight = ball.weight;
	next_ball.m_coefficient = ball.m_coefficient;
	next_ball.restitution_coefficient = ball.restitution_coefficient;
	next_ball.drag_coefficient = ball.drag_coefficient;
	next_ball.rolling_coefficient = ball.rolling_coefficient;
	next_ball.is_rolling = ball.is_rolling;
	next_ball.rotation = ball.rotation;
	next_ball.is_hit = 0;
	
	double v = sqrt(ball.vx * ball.vx  + ball.vy * ball.vy + ball.vz * ball.vz);

	double air_common_coefficient = -0.5 * env.air_density * 
		ball.drag_coefficient * PI * ball.r * ball.r * v;
	double f_air_x = air_common_coefficient * ball.vx;
	double f_air_y = air_common_coefficient * ball.vy;
	double f_air_z = air_common_coefficient * ball.vz;


	/*
	 * vx, vy, vz
	 * rx, ry, rz
	 */
	double mx = ball.vy * ball.rz - ball.vz * ball.ry;
	double my = ball.vz * ball.rx - ball.vx * ball.rz;
	double mz = ball.vx * ball.ry - ball.vy * ball.rx;
	double m = sqrt(mx * mx + my * my + mz * mz);
	double m_common_coefficient = 0.5 * env.air_density *
		ball.m_coefficient * PI * ball.r * ball.r * v * v;
	
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

	double f_g = env.gravity * ball.weight;

	double fx = f_air_x + f_m_x;
	double fy = f_air_y + f_m_y + f_g;
	double fz = f_air_z + f_m_z;
	
	double f_rolling_x = 0;
	double f_rolling_z = 0;

	if (ball.is_rolling) {
		double f_rolling = f_g * ball.rolling_coefficient;
		double hv = sqrt(ball.vx * ball.vx + ball.vz * ball.vz);
		if (hv != 0) {
			f_rolling_x = ball.vx / hv * f_rolling;
			f_rolling_z = ball.vz / hv * f_rolling;
		} 
	}

	double ax = fx / ball.weight;
	double ay = fy / ball.weight;
	double az = fz / ball.weight;

	next_ball.vx = ball.vx + ax * t;
	next_ball.vy = ball.vy + ay * t;
	next_ball.vz = ball.vz + az * t;

	double vx_with_rolling = next_ball.vx +  f_rolling_x / ball.weight * t;
	double vz_with_rolling = next_ball.vz + f_rolling_z / ball.weight * t;
	if (vx_with_rolling * next_ball.vx <= 0)
		next_ball.vx = 0;
	else
		next_ball.vx = vx_with_rolling;

	if (vz_with_rolling * next_ball.vz <= 0)
		next_ball.vz = 0;
	else
		next_ball.vz = vz_with_rolling;


	next_ball.x = 0.5 * (ball.vx + next_ball.vx) * t + ball.x;
	next_ball.y = 0.5 * (ball.vy + next_ball.vy) * t + ball.y;
	next_ball.z = 0.5 * (ball.vz + next_ball.vz) * t + ball.z;

	if ((next_ball.y - next_ball.r) <= 0) { /* hit the ground */

		next_ball.is_hit = 1;

		next_ball.y = next_ball.r;
		
		next_ball.vy = -next_ball.restitution_coefficient *
			next_ball.vy;

		if (next_ball.vy < 5 * env.gravity) {
			next_ball.vy *= next_ball.restitution_coefficient;
		}
		if (next_ball.vy + 2 * t * env.gravity < 0) {
			next_ball.is_rolling = 1;
			next_ball.y = next_ball.vy * t + next_ball.y;
		}
	}

	if (next_ball.is_rolling) {
		next_ball.is_hit = 0;
		if (next_ball.y - next_ball.r < 0.001) {
			next_ball.y = next_ball.r;
		}
		else {
			next_ball.y = next_ball.y + 0.9 * (next_ball.r - next_ball.y);
		}
		next_ball.vy = 0;
	}

	next_ball.rx = next_ball.vz;
	next_ball.rz = -next_ball.vx;
	next_ball.ry = 0;
	double r = sqrt(next_ball.rx * next_ball.rx + next_ball.rz * next_ball.rz);
	if (r != 0) {
		next_ball.rx /= r;
		next_ball.rz /= r;
	}
	else {
		next_ball.rx = 0;
		next_ball.rz = 0;
	}
	next_ball.w = sqrt(next_ball.vx * next_ball.vx + next_ball.vz * next_ball.vz) / next_ball.r;

	next_ball.rotation += next_ball.w / (2 * PI) * t * 360;

	return next_ball;
}
