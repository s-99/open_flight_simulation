#include "Fcs.h"

#include "vehicle.h"

void Fcs::step(double dt, double t)
{
}


bool Fcs::init(const json& vehicle_config, const json& sub_system_config)
{
	_vehicle->_data_pool.reg_data("de", _de, "Fcs");
	_vehicle->_data_pool.reg_data("da", _da, "Fcs");
	_vehicle->_data_pool.reg_data("dr", _dr, "Fcs");
	//_vehicle->_data_pool.reg_data("df", _df, "Fcs");
	return true;
}



bool Fcs::bind_data()
{
	std::string failed_input;
	BIND_DATA(stick_push);
	BIND_DATA(stick_right);
	BIND_DATA(pedal_right);
	if (failed_input.empty())
	{
		return true;
	}
	loge("Fcs::bind_data: {}[{}-{}] bind {} failed.\n", _class_name, _vehicle->_id, _id, failed_input);
	return false;
}
