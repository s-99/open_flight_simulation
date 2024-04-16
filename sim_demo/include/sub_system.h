#pragma once

class SimEvent;

class SubSystem
{
public:
	SubSystem() = default;
	virtual ~SubSystem() = default;

	virtual void step(double dt, double t) = 0;
	virtual void on_sim_event(SimEvent* event) {}
};
