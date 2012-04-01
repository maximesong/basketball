#include <cmath>

#define PI 3.1415
#define MIN_DOUBLE 0.00001

typedef struct {
	double weight;

/* the radius of the ball */
	double r;

/* The position of the ball */
	double x;
	double y;
	double z;

/* the velocity of transition */
	double vx;
	double vy;
	double vz;

/* the velocity of rotation */
	double w;

/* the rotation aix */
	double rx;
	double ry;
	double rz;

	double drag_coefficient;
	double m_coefficient;
	double restitution_coefficient;
	double rolling_coefficient;

	double rotation;

	bool is_rolling;
	bool is_hit;
} Ball;

typedef struct {
	double gravity;
	double air_density;
} Env;

Ball calculateXyFallingBall(const Ball &ball, const Env &env, double t);
