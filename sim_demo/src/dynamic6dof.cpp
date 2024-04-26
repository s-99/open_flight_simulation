#include <json.hpp>
#include <fstream>

#include "dynamic_6dof.h"
#include "vec3.h"
#include "matrix3x3.h"
#include "quat.h"
#include "util.h"
#include "fmtlog.h"


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
	logi("Dynamic6DOF::step\n");

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
}


using json = nlohmann::json;

bool Dynamic6DOF::init()
{
	logi("Dynamic6DOF::init\n");
	std::ifstream f("fat.json");
	json data;
	try
	{
		data = json::parse(f);
	}
	catch (nlohmann::json::parse_error& e)
	{
		loge("Parse error: {}", e.what());
		return false;
	}

	auto& dyn = data["dyn"];
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
	return true;
}