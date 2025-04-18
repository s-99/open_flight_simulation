#include "dynamic_6dof.h"

#include <json.hpp>
#include <fstream>

#include "vec3.h"
#include "matrix3x3.h"
#include "quat.h"
#include "util.h"
#include "fmtlog.h"
#include "vehicle.h"


template<typename T>
T deg2rad(T deg)
{
	return deg / 180.0 * M_PI;
}

template<typename T>
T rad2deg(T rad)
{
	return rad / M_PI * 180.0;
}


// 动力学方程组中的12个核心微分方程
auto dynamic_equation(
	const Vec3& F,			// 体轴系合力
	const Vec3& M,			// 体轴系合力距
	const Vec3& V,			// 体轴系内地速分量
	const Vec3& Omega,		// 体轴系相对于地轴系的角速度在体轴系内的分量
	const Quaternion& Q,	// 从地轴系到体轴系的四元数
	const Matrix3x3& I,		// 惯性矩阵
	const double m			// 质量
)
{
	const auto V_dot = F / m - Omega.cross(V);
	const auto Omega_dot = I.inverse() * (M - Omega.cross(I * Omega));
	// Q是从地轴系到体轴系的四元数，但是在这里我们需要的是从体轴系到地轴系的四元数，所以要取逆
	// 并且对矢量旋转与坐标轴旋转是反的，所以也要取逆
	// 两者抵消，所以这里不用取逆
	const auto x_dot = Q.rotate_vector(V);
	const auto Q_dot = Q.derivative(Omega);
	return std::make_tuple(V_dot, Omega_dot, x_dot, Q_dot);
}


auto auxiliary_equation(const Vec3& Va, const Vec3& Va_dot, double rho)
{
	double V = Va.length();
	double alpha = rad2deg(atan2(Va[2], Va[0]));
	double beta = rad2deg(asin(Va[1] / V));
	double alpha_dot = (Va[0] * Va_dot[2] - Va[2] * Va_dot[0]) / (Va[0] * Va[0] + Va[2] * Va[2]);
	double q_bar = 0.5 * rho * V * V;

	return std::make_tuple(V, alpha, beta, alpha_dot, q_bar);
}


void Dynamic6DOF::step(double dt, double t)
{
	_binder.update();

	Vec3 Fg = _quat.rotate_vector({ 0, 0, _mass * GRAVITY0 }); 

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

	_V += V_dot * dt;
	_Omega += Omega_dot * dt;
	_pos += x_dot * dt;
	_quat += Q_dot * dt;
	_quat.to_euler_degree(_euler);

	_Va = _V;

	auto [density, sound_speed] = cal_std_atm(-_pos[2]);
	std::tie(_Vel, _alpha, _beta, _alpha_dot, _q_bar) = auxiliary_equation(_Va, V_dot, density);
	_mach = _Vel / sound_speed;

	_h = -_pos[2];

	_recorder.update(t);
}


bool Dynamic6DOF::init(const json& vehicle_config, const json& sub_system_config)
{
	logi("Dynamic6DOF::init\n");

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

	_recorder.init(read_json_string(sub_system_config, "recorder_filename"));

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
	reg_data("ps", _Omega[0]);
	reg_data("qs", _Omega[1]);
	reg_data("rs", _Omega[2]);
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

	return true;
}


bool Dynamic6DOF::bind_data()
{
	// 绑定数据
	std::string failed_input;
	BIND_DATA_NAME("Fx", _Fa[0]);
	BIND_DATA_NAME("Fy", _Fa[1]);
	BIND_DATA_NAME("Fz", _Fa[2]);
	BIND_DATA_NAME("L", _Ma[0]);
	BIND_DATA_NAME("M", _Ma[1]);
	BIND_DATA_NAME("N", _Ma[2]);
	if (!failed_input.empty())
	{
		loge("Dynamic6DOF::bind_data: {}[{}-{}] bind {} failed.\n", _class_name, _vehicle->_id, _id, failed_input);
		return false;
	}
	_recorder.update(0);
	return true;
}
