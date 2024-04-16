#pragma once
#include "sub_system.h"

struct ControlSurface
{
	double _de, _da, _dr, _df;
};


class Aero : public SubSystem
{
public:
	Aero() = default;
	virtual ~Aero() = default;

	void step(double dt, double t) override;

	// inputs
	ControlSurface _control_surface;

	// outputs
	double _Fx, _Fy, _Fz;
	double _L, _M, _N;
};
