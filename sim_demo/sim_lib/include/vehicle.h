#pragma once

#include <vector>

#include "data_pool.h"
#include "sub_system.h"

class SimEvent;

class Vehicle
{
public:
	Vehicle() = default;
	virtual ~Vehicle() = default;

	virtual bool init(const json& vehicle_config);
	virtual void step(double dt, double t);
	virtual void on_sim_event(SimEvent* event) {}

	std::vector<SubSystem*> _sub_systems;
	DataPool _data_pool;

	json _data_file;

	std::string _class_name;
	int _id = 0;

	std::string _3d_model;
};
