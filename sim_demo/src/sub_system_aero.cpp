#include "sub_system_aero.h"
#include "vehicle.h"

#include <cstdio>

#include "fmtlog.h"
#include "util.h"


void SubSystemAero::step(double dt, double t)
{
	_binder.update();

	_model.eval();

	_Fx = _model.get_variable("X");
	_Fy = _model.get_variable("Y");
	_Fz = _model.get_variable("Z");
	_L = _model.get_variable("L");
	_M = _model.get_variable("M");
	_N = _model.get_variable("N");

	_recorder.update(t);
}


bool SubSystemAero::init(const json& vehicle_config, const json& sub_system_config)
{
	if (!_model.parse(_vehicle->_data_file["aero"]))
	{
		loge("parse aero model failed\n");
		return false;
	}

	logi("load aero model success, content=\n{}\n", _model.dump());

	_vehicle->_data_pool.reg_data("X", _Fx, "SubSystemAero");
	_vehicle->_data_pool.reg_data("Y", _Fy, "SubSystemAero");
	_vehicle->_data_pool.reg_data("Z", _Fz, "SubSystemAero");
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
	std::string bind_failed_names;
	for (auto* v : _model._inputs)
	{
		if (!_binder.bind(_vehicle->_data_pool, v->_name, v->_value))
		{
			bind_failed_names += v->_name + ",";
		}
	}
	if (!bind_failed_names.empty())
	{
		loge("bind data failed: {}", bind_failed_names);
		return false;
	}
	_recorder.update(0);
	return true;
}
