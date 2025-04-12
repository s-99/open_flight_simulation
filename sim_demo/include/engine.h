#pragma once
#include "sub_system.h"


class Engine : public SubSystem
{
public:
	Engine()
	{
		_class_name = "Engine";
	}
	virtual ~Engine() = default;

	bool bind_data() override { return true; }
	bool init(const json& vehicle_config, const json& sub_system_config) override { return true; }
	void step(double dt, double t) override;
};
