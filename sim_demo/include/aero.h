#pragma once
#include "data_pool.h"
#include "sub_system.h"


class Aero : public SubSystem
{
public:
	Aero() = default;
	virtual ~Aero() = default;

	bool init() override;
	bool bind_data() override;
	void step(double dt, double t) override;

	// inputs
	double _de, _da, _dr, _df;

	// outputs
	double _Fx, _Fy, _Fz;
	double _L, _M, _N;

	DataBinder _binder;
};
