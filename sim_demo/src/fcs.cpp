#include "Fcs.h"
#include "ode_solver_adams_bashforth.h"
#include "ode_solver_runge_kutta.h"

#include "vehicle.h"


void Fcs::step(double dt, double t)
{
	_binder.update();
	_fcs_model.step(dt, t);
	_recorder.update(t);
}


bool Fcs::init(const json& vehicle_config, const json& sub_system_config)
{
	if (!_fcs_model.init(_vehicle->_data_file["fcs"]))
	{
		loge("Fcs::init: {}[{}-{}] init fcs model failed.\n", _class_name, _vehicle->_id, _id);
		return false;
	}

	// 积分器初始化
	// "runge-kutta", "adams-bashforth"
	auto solver_name = read_json_string(sub_system_config, "solver");
	auto solver_order = read_json_int(sub_system_config, "solver_order", 1);
	if (solver_name == "runge-kutta")
	{
		_fcs_model.set_solver(new OdeSolverRungeKuttaN(solver_order));
	}
	else if (solver_name == "adams-bashforth")
	{
		_fcs_model.set_solver(new OdeSolverAdamsBashforthN(solver_order));
	}

	int i = 0;
	for (auto& o : _fcs_model._output_sig)
	{
		_vehicle->_data_pool.reg_data(o._name, _fcs_model.get_output(i), "Fcs");
		i++;
	}

	_recorder.init(read_json_string(sub_system_config, "recorder_filename"));
	for (auto* block : _fcs_model._blocks)
	{
		_recorder.reg(block->_name, block->get_output(0));
	}

	return true;
}


bool Fcs::bind_data()
{
	std::string failed_input;
	for (auto& i : _fcs_model._input_sig)
	{
		if (!_binder.bind(_vehicle->_data_pool, i._name, i._value))
		{
			failed_input += i._name + ",";
		}
	}

	if (!failed_input.empty())
	{
		loge("SubSystem6DofAero::bind_data: {}[{}-{}] bind {} failed.\n", _class_name, _vehicle->_id, _id, failed_input);
		return false;
	}
	_recorder.update(0);
	return true;
}
