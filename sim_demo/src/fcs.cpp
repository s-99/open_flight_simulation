#include "Fcs.h"

#include <cstdio>

#include "vehicle.h"

void Fcs::step(double dt, double t)
{
	printf("Fcs::step\n");
}


bool Fcs::init()
{
	_vehicle->_data_pool.reg_data("de", _de, "Fcs");
	_vehicle->_data_pool.reg_data("da", _da, "Fcs");
	_vehicle->_data_pool.reg_data("dr", _dr, "Fcs");
	//_vehicle->_data_pool.reg_data("df", _df, "Fcs");
	return true;
}



bool Fcs::bind_data()
{
	BIND_DATA(stick_push);
	BIND_DATA(stick_right);
	BIND_DATA(pedal_right);
	return true;
}
