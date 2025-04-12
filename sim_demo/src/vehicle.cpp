#include "vehicle.h"

#include <fstream>


bool Vehicle::init(const json& vehicle_config)
{
	logi("Vehicle::init\n");

	auto data_file = vehicle_config["data_file"].get<std::string>();
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

	return true;
}
