#pragma once

#include "vehicle.h"

#include <string>


// 仿真引擎，用于管理仿真对象、驱动仿真运行
class SimEngine
{
public:
	SimEngine() = default;
	virtual ~SimEngine()
	{
		for (auto* vehicle : _vehicles)
		{
			delete vehicle;
		}
	}
	bool parse_mission(const std::string& filename);

	SubSystem* create_sub_system(Vehicle* parent, const json& vehicle_config, const json& sub_system_config, const int id);
	Vehicle* create_vehicle(const json& vehicle_config, const int id);

	bool step();

	std::vector<Vehicle*> _vehicles;

	double _time = 0.0;
	double _time_step = 0.01;
	double _max_time = 0.0;
};
