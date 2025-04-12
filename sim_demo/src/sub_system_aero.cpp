#include "sub_system_aero.h"
#include "vehicle.h"

#include <cstdio>

#include "fmtlog.h"
#include "util.h"

void SubSystemAero::step(double dt, double t)
{
	logi("SubSystemAero::step\n");

	_binder.update();

	_model.eval();

	_Fx = _model.get_variable("Fx");
	_Fy = _model.get_variable("Fy");
	_Fz = _model.get_variable("Fz");
	_L = _model.get_variable("L");
	_M = _model.get_variable("M");
	_N = _model.get_variable("N");

	_recorder.update(t);
}


bool SubSystemAero::init(const json& vehicle_config, const json& sub_system_config)
{
	_model.parse(_vehicle->_data_file);

	logi("load aero model success, content=\n{}\n", _model.dump());

	_vehicle->_data_pool.reg_data("Fx", _Fx, "SubSystemAero");
	_vehicle->_data_pool.reg_data("Fy", _Fy, "SubSystemAero");
	_vehicle->_data_pool.reg_data("Fz", _Fz, "SubSystemAero");
	_vehicle->_data_pool.reg_data("L", _L, "SubSystemAero");
	_vehicle->_data_pool.reg_data("M", _M, "SubSystemAero");
	_vehicle->_data_pool.reg_data("N", _N, "SubSystemAero");

	_recorder.init(read_json_string(sub_system_config, "recorder_filename"));

	for (auto* input: _model._inputs)
	{
		_recorder.reg("i/" + input->_name, input->_value);
	}

	for (auto* c: _model._cells)
	{
		_recorder.reg(c->_name, c->_value);
	}

	return true;
}


bool SubSystemAero::bind_data()
{
	for (auto* v : _model._inputs)
	{
		if (!_binder.bind(_vehicle->_data_pool, v->_name, v->_value))
		{
			loge("bind {} failed\n", v->_name);
			return false;
		}
	}
	return true;
}
