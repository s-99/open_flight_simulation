#pragma once

#include "json.hpp"

class SimEvent;
class Vehicle;

using nlohmann::json;

class SubSystem
{
public:
	SubSystem() = default;
	virtual ~SubSystem() = default;

	virtual bool init(const json& vehicle_config, const json& sub_system_config) = 0;
	virtual bool bind_data() = 0;
	virtual void step(double dt, double t) = 0;
	virtual void on_sim_event(SimEvent* event) {}

	Vehicle* _vehicle = nullptr;

	std::string _class_name;
	int _id = 0;

};
