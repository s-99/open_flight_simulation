#include "aero.h"
#include "vehicle.h"

#include <cstdio>

void Aero::step(double dt, double t)
{
	printf("Aero::step\n");

	_binder.update();

}


bool Aero::init()
{
	_vehicle->_data_pool.reg_data("Fx", _Fx, "Aero");
	_vehicle->_data_pool.reg_data("Fy", _Fy, "Aero");
	_vehicle->_data_pool.reg_data("Fz", _Fz, "Aero");
	_vehicle->_data_pool.reg_data("L", _L, "Aero");
	_vehicle->_data_pool.reg_data("M", _M, "Aero");
	_vehicle->_data_pool.reg_data("N", _N, "Aero");
	return true;
}


bool Aero::bind_data()
{
	BIND_DATA(de);
	BIND_DATA(da);
	BIND_DATA(dr);
	BIND_DATA(df);
	return true;
}
