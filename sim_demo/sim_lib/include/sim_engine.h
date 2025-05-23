#pragma once
#include <windows.h>

#include "vehicle.h"
#include <string>


typedef SubSystem* (*func_create_sub_system)();
typedef void (*func_destroy_sub_system)(SubSystem*);


struct SubSystemDllWrapper
{
	HMODULE _dll = nullptr;
	func_create_sub_system _func_create = nullptr;
	func_destroy_sub_system _func_destroy = nullptr;
};


// 仿真引擎，用于管理仿真对象、驱动仿真运行
class SimEngine
{
public:
	SimEngine() = default;
	virtual ~SimEngine();

	bool parse_mission(const std::string& filename);

	SubSystem* create_sub_system(Vehicle* parent, const json& vehicle_config, const json& sub_system_config, const int id);
	Vehicle* create_vehicle(const json& vehicle_config, const int id);

	bool step();

	bool load_dll(const std::string& type);

	std::map<std::string, SubSystemDllWrapper> _dll_wrappers;

	std::vector<Vehicle*> _vehicles;

	double _time = 0.0;
	double _time_step = 0.01;
	double _max_time = 0.0;
};
