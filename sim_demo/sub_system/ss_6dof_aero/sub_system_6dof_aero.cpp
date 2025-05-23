#include "sub_system_6dof_aero.h"

#include <json.hpp>
#include <fstream>

#include "dynamic_equation.h"
#include "fmtlog.h"
#include "ode_solver_adams_bashforth.h"
#include "util.h"
#include "vehicle.h"


DLL_EXPORT
SubSystem* create_sub_system()
{
	return new SubSystem6DofAero();
}


DLL_EXPORT
void destroy_sub_system(SubSystem* ss)
{
	delete ss;
}


SubSystem6DofAero::~SubSystem6DofAero()
{
	delete _solver;
}


void SubSystem6DofAero::step(double dt, double t)
{
	ODE::step(dt, t);

	_recorder.update(t);
}


bool SubSystem6DofAero::init(const json& vehicle_config, const json& sub_system_config)
{
	logi("SubSystem6DofAero::init\n");

	// 质量及惯矩数据初始化
	auto& dyn = _vehicle->_data_file["dyn"];
	_mass = dyn["mass"];
	double Ix = dyn["Ix"];
	double Iy = dyn["Iy"];
	double Iz = dyn["Iz"];
	double Ixz = dyn["Ixz"];
	_I = {
			Ix, 0, -Ixz,
			0, Iy, 0,
			-Ixz, 0, Iz
		};

	// 初始化气动模型
	if (!_model.parse(_vehicle->_data_file["aero"]))
	{
		loge("parse aero model failed\n");
		return false;
	}

	_recorder.init(read_json_string(sub_system_config, "recorder_filename"));

	logi("load aero model success, content=\n{}\n", _model.dump());

	// 积分器初始化
	// "runge-kutta", "adams-bashforth"
	auto solver_name = read_json_string(sub_system_config, "solver");
	auto solver_order = read_json_int(sub_system_config, "solver_order", 1);
	if (solver_name == "runge-kutta")
	{
		set_rk_solver(solver_order);
	}
	else if (solver_name == "adams-bashforth")
	{
		set_solver(new OdeSolverAdamsBashforthN(solver_order));
	}

	_pos = read_json_vec3(vehicle_config, "init_pos", {0, 0, -10});
	_euler[2] = read_json_double(vehicle_config, "init_psi", 0);
	_quat.from_euler_degree(_euler);
	_V[0] = read_json_double(vehicle_config, "init_velocity", 0);
	_Vel = _V[0];

	_Va = _V;
	_h = -_pos[2];

	auto [density, sound_speed] = cal_std_atm(-_pos[2]);
	std::tie(_Vel, _alpha, _beta, _alpha_dot, _q_bar) = auxiliary_equation(_Va, {0, 0, 0}, density);
	_mach = _Vel / sound_speed;

	reg_data("u", _V[0]);
	reg_data("v", _V[1]);
	reg_data("w", _V[2]);
	reg_data("p", _Omega[0]);
	reg_data("q", _Omega[1]);
	reg_data("r", _Omega[2]);
	reg_data("ps", _Omega_s[0]);
	reg_data("qs", _Omega_s[1]);
	reg_data("rs", _Omega_s[2]);
	reg_data("x", _pos[0]);
	reg_data("y", _pos[1]);
	reg_data("z", _pos[2]);
	reg_data("q0", _quat[0]);
	reg_data("q1", _quat[1]);
	reg_data("q2", _quat[2]);
	reg_data("q3", _quat[2]);
	reg_data("phi", _euler[0]);
	reg_data("theta", _euler[1]);
	reg_data("psi", _euler[2]);
	reg_data("vel", _Vel);
	reg_data("alpha", _alpha);
	reg_data("beta", _beta);
	reg_data("alpha_d", _alpha_dot);
	reg_data("qq", _q_bar);
	reg_data("mach", _mach);
	reg_data("h", _h);

	reg_data("Fax", _Fa[0]);
	reg_data("Fay", _Fa[1]);
	reg_data("Faz", _Fa[2]);

	reg_data("Max", _Ma[0]);
	reg_data("May", _Ma[1]);
	reg_data("Maz", _Ma[2]);

	for (auto* input : _model._inputs)
	{
		_recorder.reg("i/" + input->_name, input->_value);
	}

	for (auto* c : _model._cells)
	{
		_recorder.reg("v/" + c->_name, c->_value);
	}

	array_d x(13);
	x[0] = _V[0];
	x[1] = _V[1];
	x[2] = _V[2];
	x[3] = _Omega[0];
	x[4] = _Omega[1];
	x[5] = _Omega[2];
	x[6] = _pos[0];
	x[7] = _pos[1];
	x[8] = _pos[2];
	x[9] = _quat[0];
	x[10] = _quat[1];
	x[11] = _quat[2];
	x[12] = _quat[3];

	ODE::set_state(x);

	return true;
}


bool SubSystem6DofAero::bind_data()
{
	// 绑定数据
	std::string failed_input;

	std::string bind_failed_names;
	for (auto* v : _model._inputs)
	{
		if (!_aero_binder.bind(_vehicle->_data_pool, v->_name, v->_value))
		{
			bind_failed_names += v->_name + ",";
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


// state variables (x):
// x[0,1,2] = u,v,w
// x[3,4,5] = p,q,r
// x[6,7,8] = x,y,z
// x[9,10,11,12] = q0,q1,q2,q3
array_d SubSystem6DofAero::derivative(const double t, const array_d& x)
{
	// 根据状态变量计算当前的飞行参数
	_V = { x[0], x[1], x[2] };
	_Omega = { x[3], x[4], x[5] };
	_pos = { x[6], x[7], x[8] };
	_quat = { x[9], x[10], x[11], x[12] };

	_quat.to_euler_degree(_euler);

	_Va = _V;
	auto x_d =  get_dot();
	Vec3 V_d = { x_d[0], x_d[1], x_d[2] };

	auto [density, sound_speed] = cal_std_atm(-_pos[2]);
	std::tie(_Vel, _alpha, _beta, _alpha_dot, _q_bar) = auxiliary_equation(_Va, V_d, density);
	_mach = _Vel / sound_speed;

	_h = -_pos[2];

	const double cosa = cos(deg2rad(_alpha));
	const double sina = sin(deg2rad(_alpha));
	_Omega_s = {
		_Omega[0] * cosa + _Omega[2] * sina,
		_Omega[1],
		_Omega[2] * cosa - _Omega[0] * sina
	};

	_aero_binder.update();
	_model.eval();

	_Fa[0] = _model.get_variable("X");
	_Fa[1] = _model.get_variable("Y");
	_Fa[2] = _model.get_variable("Z");
	_Ma[0] = _model.get_variable("L");
	_Ma[1] = _model.get_variable("M");
	_Ma[2] = _model.get_variable("N");

	Vec3 Fg = _quat.inverse().rotate_vector({ 0, 0, _mass * GRAVITY0 });
	Vec3 F = _Fa + _Fe + Fg;
	Vec3 M = _Ma + _Me;
	auto [V_dot, Omega_dot, x_dot, Q_dot] = dynamic_equation(
		F,
		M,
		_V,
		_Omega,
		_quat,
		_I,
		_mass
	);
	return {
		V_dot[0], V_dot[1], V_dot[2],
		Omega_dot[0], Omega_dot[1], Omega_dot[2],
		x_dot[0], x_dot[1], x_dot[2],
		Q_dot[0], Q_dot[1], Q_dot[2], Q_dot[3]
	};
}


void SubSystem6DofAero::do_output(const double t, const array_d& x, const array_d& x_d)
{
	_V = { x[0], x[1], x[2] };
	_Omega = { x[3], x[4], x[5] };
	_pos = { x[6], x[7], x[8] };
	_quat = { x[9], x[10], x[11], x[12] };

	_quat.to_euler_degree(_euler);

	_Va = _V;

	Vec3 V_dot = { x_d[0], x_d[1], x_d[2] };

	auto [density, sound_speed] = cal_std_atm(-_pos[2]);
	std::tie(_Vel, _alpha, _beta, _alpha_dot, _q_bar) = auxiliary_equation(_Va, V_dot, density);
	_mach = _Vel / sound_speed;

	_h = -_pos[2];

	const double cosa = cos(deg2rad(_alpha));
	const double sina = sin(deg2rad(_alpha));
	_Omega_s = {
		_Omega[0] * cosa + _Omega[2] * sina,
		_Omega[1],
		_Omega[2] * cosa - _Omega[0] * sina
	};
}
