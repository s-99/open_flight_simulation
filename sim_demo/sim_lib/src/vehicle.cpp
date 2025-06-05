#include "vehicle.h"

#include <fstream>

#include "util.h"


bool Vehicle::init(const json& vehicle_config)
{
	logi("Vehicle::init\n");

	auto data_file = read_json_string(vehicle_config, "data_file");
	std::ifstream f(data_file);
	try
	{
		_data_file = json::parse(f);
	}
	catch (json::parse_error& e)
	{
		loge("Parse error: {}", e.what());
		return false;
	}

	_3d_model = read_json_string(vehicle_config, "3d_model");

	return true;
}


void Vehicle::step(double dt, double t)
{
	auto t0 = std::chrono::high_resolution_clock::now();
	fmt::memory_buffer buf;
	for (auto* sub_system : _sub_systems)
	{
		sub_system->step(dt, t);
		const auto t1 = std::chrono::high_resolution_clock::now();
		const auto dt = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
		fmt::format_to(std::back_inserter(buf), "[{}]: {:<20} {:.3f}ms\n",
			sub_system->_id, sub_system->_class_name, static_cast<double>(dt.count()) / 1000.0);
		t0 = t1;
	}
	logi("Vehicle::step: sub system step() time [t={:.3f}]: \n{}", t, fmt::to_string(buf));
}
