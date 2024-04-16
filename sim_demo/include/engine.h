#pragma once
#include "sub_system.h"


class Engine : public SubSystem
{
public:
	Engine() = default;
	virtual ~Engine() = default;

	void step(double dt, double t) override;

};
