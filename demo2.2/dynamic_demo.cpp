#include "vec3.h"
#include "matrix3x3.h"

auto dynamic_equation(const Vec3& F, const Vec3& M, const Vec3& V, const Vec3& Omega, const Matrix3x3& I, const double m)
{
	const auto V_dot = F / m - Omega ^ V;
	const auto Omega_dot = I.inverse() * (M - Omega ^ (I * Omega));
	return std::make_tuple(V_dot, Omega_dot);
}