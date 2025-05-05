#include "ode_solver_adams_bashforth.h"

#include <vector>

#include "fmtlog.h"


// ============================================================================
// Adams–Bashforth系数表，来源：Numerical Methods for Ordinary Differential Equations， P.116, Table2 44(I)
// 外层索引是阶数-1，内层是历史长度
// Adams-Bashforth方法的系数是根据历史长度来选取的，历史长度与阶数一致
// 但是在启动过程中，历史长度可能小于阶数，因此需要根据历史长度来选取系数
constexpr double ADAMS_B[4][4] = {
	{1.0, 0.0, 0.0, 0.0},									// order 1
	{1.5, -0.5, 0.0, 0.0},									// order 2
	{23.0 / 12.0, -16.0 / 12.0, 5.0 / 12.0, 0.0},			// order 3
	{55.0 / 24.0, -59.0 / 24.0, 37.0 / 24.0, -9.0 / 24.0}	// order 4
};


OdeSolverAdamsBashforthN::OdeSolverAdamsBashforthN(const int order)
{
	_order = order;
	if (order < 1 || order > 4)
	{
		logw("OdeSolverAdamsBashforthN::OdeSolverAdamsBashforthN: order {} out of range [1, 4], set order to 1\n",
			order);
		_order = 1;
	}
}


void OdeSolverAdamsBashforthN::integrate(ODE* ode, const double dt, const double t, array_d& x)
{
	// 计算当前状态的导数
	auto f = ode->derivative(t, x);
	_dot_history.push_front(f);

	// 启动过程使用低价的Adams-Bashforth方法，
	// 处理方式就是只有当历史时间长度大于等于阶数时才使用给定阶数
	if (_dot_history.size() > _order)
	{
		_dot_history.pop_back();
	}

	// 使用Adams-Bashforth公式进行积分
	// d是加权平均后的导数
	array_d d(0.0, x.size());
	// ADAMS_B按照当前历史长度来选取相应的系数，
	// 用于处理启动过程中历史长度的情况
	const double* b = ADAMS_B[_dot_history.size() - 1];
	for (size_t i = 0; i < _dot_history.size(); i++)
	{
		d += _dot_history[i] * b[i];
	}

	x += d * dt;
	ode->set_dot(f);
}
