#include <json.hpp>
#include <fstream>

#include "dynamic_6dof.h"
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


// ����ѧ�������е�12������΢�ַ���
auto dynamic_equation(
	const Vec3& F,			// ����ϵ����
	const Vec3& M,			// ����ϵ������
	const Vec3& V,			// ����ϵ�ڵ��ٷ���
	const Vec3& Omega,		// ����ϵ����ڵ���ϵ�Ľ��ٶ�������ϵ�ڵķ���
	const Quaternion& Q,	// �ӵ���ϵ������ϵ����Ԫ��
	const Matrix3x3& I,		// ���Ծ���
	const double m			// ����
)
{
	const auto V_dot = F / m - Omega ^ V;
	const auto Omega_dot = I.inverse() * (M - Omega ^ (I * Omega));
	// Q�Ǵӵ���ϵ������ϵ����Ԫ��������������������Ҫ���Ǵ�����ϵ������ϵ����Ԫ��������Ҫȡ��
	// ���Ҷ�ʸ����ת����������ת�Ƿ��ģ�����ҲҪȡ��
	// ���ߵ������������ﲻ��ȡ��
	const auto x_dot = Q ^ V;
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
	auto [V_dot, Omega_dot, x_dot, Q_dot] = dynamic_equation(
		{ _Fx, _Fy, _Fz },
		{ _L, _M, _N },
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

	_Va = _V;

	auto [density, sound_speed] = cal_std_atm(-_pos[2]);
	std::tie(_Vel, _alpha, _beta, _alpha_dot, _q_bar) = auxiliary_equation(_Va, V_dot, density);
	_mach = _Vel / sound_speed;

	_h = -_pos[2];
}


bool Dynamic6DOF::init(const json& vehicle_config, const json& sub_system_config)
{
	logi("Dynamic6DOF::init\n");

	// �������߾����ݳ�ʼ��
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
	_V[0] = read_json_double(vehicle_config, "init_velocity", 0);
	_Vel = _V[0];

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
	reg_data("vel", _Vel);
	reg_data("alpha", _alpha);
	reg_data("beta", _beta);
	reg_data("alpha_d", _alpha_dot);
	reg_data("qq", _q_bar);
	reg_data("mach", _mach);
	reg_data("h", _h);

	return true;
}