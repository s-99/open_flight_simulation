#pragma once

class SimEvent;
class Vehicle;

class SubSystem
{
public:
	SubSystem() = default;
	virtual ~SubSystem() = default;

	virtual bool init() = 0;
	virtual bool bind_data() = 0;
	virtual void step(double dt, double t) = 0;
	virtual void on_sim_event(SimEvent* event) {}

	Vehicle* _vehicle = nullptr;
};
