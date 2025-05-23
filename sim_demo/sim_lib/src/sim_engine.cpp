#include "sim_engine.h"

#include <fstream>

#include "aircraft.h"
#include "sub_system.h"

#include "util.h"


bool SimEngine::parse_mission(const std::string& filename)
{
	std::ifstream f(filename);
	json data;
	try
	{
		data = json::parse(f);
	}
	catch (json::parse_error& e)
	{
		loge("Parse error: {}", e.what());
		return false;
	}

	if (read_json_string(data, "type") != "mission")
	{
		loge("Parse error: not a mission file: {}", filename);
		return false;
	}

	_time_step = read_json_double(data, "time_step", 0.01);
	_max_time = read_json_double(data, "max_time", 10.0);

	int id = 0;
	for (auto& vehicle_config : data["vehicles"])
	{
		auto* vehicle = create_vehicle(vehicle_config, id);
		if (vehicle)
		{
			_vehicles.push_back(vehicle);
		}
		else
		{
			loge("failed to create vehicle: [{}]\n", id);
			return false;
		}
		id++;
	}

	return true;
}


Vehicle* SimEngine::create_vehicle(const json& vehicle_config, const int id)
{
	auto type = vehicle_config["type"].get<std::string>();

	Vehicle* vehicle = nullptr;
	if (type == "aircraft")
	{
		vehicle = new Aircraft();
	}
	else
	{
		loge("Unknown vehicle type: {}", type);
		return nullptr;
	}

	vehicle->_id = id;

	if (!vehicle->init(vehicle_config))
	{
		loge("Vehicle init failed: {}", type);
		delete vehicle;
		return nullptr;
	}

	int ss_id = 0;
	for (auto& sub_system_config : vehicle_config["sub_systems"])
	{
		auto* sub_system = create_sub_system(vehicle, vehicle_config, sub_system_config, ss_id++);
		if (sub_system)
		{
			vehicle->_sub_systems.push_back(sub_system);
		}
		else
		{
			loge("failed to create sub system\n");
			delete vehicle;
			return nullptr;
		}
	}
	// 绑定数据
	for (auto* sub_system : vehicle->_sub_systems)
	{
		if (!sub_system->bind_data())
		{
			loge("failed to bind data for sub system: {}[{}-{}]\n", 
				sub_system->_class_name, vehicle->_id, sub_system->_id);
			delete vehicle;
			return nullptr;
		}
	}

	return vehicle;
}


SubSystem* SimEngine::create_sub_system(Vehicle* parent, const json& vehicle_config, const json& sub_system_config, const int id)
{
	//auto type = sub_system_config["type"].get<std::string>();
	//SubSystem* sub_system = nullptr;
	//if (type == "fcs")
	//{
	//	sub_system = new Fcs();
	//}
	//else if (type == "engine")
	//{
	//	sub_system = new Engine();
	//}
	//else if (type == "aerodynamic")
	//{
	//	sub_system = new SubSystemAero();
	//}
	//else if (type == "dynamic6dof")
	//{
	//	sub_system = new Dynamic6DOF();
	//}
	//else if (type == "sig_generator")
	//{
	//	sub_system = new SigGenerator();
	//}
	//else if (type == "6dof_aero")
	//{
	//	sub_system = new SubSystem6DofAero();
	//}
	//else
	//{
	//	loge("Unknown sub system type: {}\n", type);
	//	return nullptr;
	//}

	//sub_system->_id = id;
	//sub_system->_vehicle = parent;
	//if (!sub_system->init(vehicle_config, sub_system_config))
	//{
	//	loge("Sub system init failed: {}\n", type);
	//	delete sub_system;
	//	return nullptr;
	//}

	//return sub_system;
	return nullptr;
}


bool SimEngine::step()
{
	_time += _time_step;
	if (_time > _max_time)
	{
		return false;
	}
	logi("SimEngine::step: {:.3f}\n", _time);
	auto t0 = std::chrono::high_resolution_clock::now();
	fmt::memory_buffer buf;
	for (auto* vehicle : _vehicles)
	{
		vehicle->step(_time_step, _time);
		const auto t1 = std::chrono::high_resolution_clock::now();
		const auto dt = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
		fmt::format_to(std::back_inserter(buf), "[{}]: {:<20} {:.3f}ms\n",
			vehicle->_id, vehicle->_class_name, static_cast<double>(dt.count()) / 1000.0);
		t0 = t1;
	}
	logi("SimEngine::step: vehicle step() time [t={:.3f}]: \n{}", _time, fmt::to_string(buf));
	return true;
}
