#include "vec3.h"
#include "matrix3x3.h"
#include "quat.h"

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