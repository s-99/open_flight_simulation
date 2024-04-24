#pragma once
#include "sub_system.h"


class Engine : public SubSystem
{
public:
	Engine() = default;
	virtual ~Engine() = default;

	bool bind_data() override { return true; }
	bool init() override { return true; }
	void step(double dt, double t) override;

};
