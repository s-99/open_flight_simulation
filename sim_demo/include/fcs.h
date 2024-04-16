#pragma once
#include "aero.h"
#include "sub_system.h"


class Fcs : public SubSystem
{
public:
	Fcs() = default;
	virtual ~Fcs() = default;

	void step(double dt, double t) override;

	// outputs
	ControlSurface _control_surface;
};
