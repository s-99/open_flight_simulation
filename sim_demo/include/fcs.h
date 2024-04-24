#pragma once
#include "aero.h"
#include "sub_system.h"


class Fcs : public SubSystem
{
public:
	Fcs() = default;
	virtual ~Fcs() = default;

	bool bind_data() override;
	bool init() override;
	void step(double dt, double t) override;

	// inputs
	double _stick_push, _stick_right, _pedal_right;

	// outputs
	double _de, _da, _dr, _df;

	DataBinder _binder;
};
