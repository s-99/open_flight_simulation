#pragma once
#include "sub_system.h"
#include "vec3.h"
#include "quat.h"
#include "matrix3x3.h"


class Dynamic6DOF : public SubSystem
{
public:
	Dynamic6DOF() {}
	virtual ~Dynamic6DOF() = default;

	bool bind_data() override { return true; }
	bool init() override;
	void step(double dt, double t) override;

	// inputs
	double _Fx, _Fy, _Fz;
	double _L, _M, _N;

	// outputs
	Vec3 _V;
	Vec3 _Omega;
	Vec3 _pos;
	Vec3 _Va;
	Vec3 _euler;
	Quaternion _quat;
	double _Vel;
	double _alpha;
	double _beta;
	double _alpha_dot;
	double _q_bar;
	double _mach;

	// data
	Matrix3x3 _I;
	double _mass;
};
