#include "EnvModel.h"

class BallModel {
public:
	BallModel(double weight);
private:
	double m_weight;

/* the radius of the ball */
	double m_r;

/* The position of the ball */
	double m_x;
	double m_y;
	double m_z;

/* the velocity of transition */
	double m_vx;
	double m_vy;
	double m_vz;

/* the velocity of rotation */
	double m_w;

/* the rotation aix */
	double m_rx;
	double m_ry;
	double m_rz;

	double m_drag_const;
	double m_m_const;
	double m_restitution_const;
	double m_rolling_const;

	double m_rotation;

	bool m_is_almost_rolling;
	bool m_is_rolling;
	bool m_is_hit;
};
