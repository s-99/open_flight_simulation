#pragma once

#include "data_pool.h"
#include "sub_system.h"


class Fcs : public SubSystem
{
public:
	Fcs()
	{
		_class_name = "Fcs";
	}
	~Fcs() override = default;

	bool bind_data() override;
	bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

	// inputs
	double _stick_push, _stick_right, _pedal_right;

	// outputs
	double _de, _da, _dr, _df;

	DataBinder _binder;
};
