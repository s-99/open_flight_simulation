#pragma once
#include "vehicle.h"

class Aircraft : public Vehicle
{
public:
	Aircraft() = default;
	virtual ~Aircraft() = default;

	void step(double dt, double t) override;
	void on_sim_event(SimEvent* event) override;
};
