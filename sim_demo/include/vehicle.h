#pragma once
#include <vector>

#include "data_pool.h"
#include "sub_system.h"

class SimEvent;

class Vehicle
{
public:
	Vehicle() = default;
	virtual ~Vehicle()
	{
		for (auto* ss: _sub_systems)
		{
			delete ss;
		}
	}

	virtual bool init(const json& vehicle_config);
	virtual void step(double dt, double t)
	{
		for (auto* sub_system : _sub_systems)
		{
			sub_system->step(dt, t);
		}
	}
	virtual void on_sim_event(SimEvent* event) {}

	std::vector<SubSystem*> _sub_systems;
	DataPool _data_pool;

	json _data_file;

	std::string _class_name;
	int _id = 0;
};
