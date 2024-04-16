#pragma once
#include <vector>

#include "sub_system.h"

class SimEvent;

class Vehicle
{
public:
	Vehicle() = default;
	virtual ~Vehicle() = default;

	virtual void step(double dt, double t)
	{
		for (auto* sub_system : _sub_systems)
		{
			sub_system->step(dt, t);
		}
	}
	virtual void on_sim_event(SimEvent* event) {}

	std::vector<SubSystem*> _sub_systems;
};
