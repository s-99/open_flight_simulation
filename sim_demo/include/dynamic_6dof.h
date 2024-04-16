#pragma once
#include "sub_system.h"


class Dynamic6DOF : public SubSystem
{
public:
	Dynamic6DOF() = default;
	virtual ~Dynamic6DOF() = default;

	void step(double dt, double t) override;

	// inputs
	double _Fx, _Fy, _Fz;
	double _L, _M, _N;

	// outputs
	double _u, _v, _w;
	double _p, _q, _r;
	// ......
};
