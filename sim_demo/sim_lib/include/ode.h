#pragma once

#include <valarray>

typedef std::valarray<double> array_d;

class ODE;


// 常微分方程求解器接口
class OdeSolver
{
public:
	virtual ~OdeSolver() = default;
	virtual void integrate(ODE* ode, const double dt, const double t, array_d& x) = 0;
};


//常微分方程组, Ordinary Differential Equation
// 微分方程：dot(x) = f(x, u)
// 输出方程：y = g(x)
class ODE
{
private:
	array_d _x;
	array_d _x_dot;

public:
	OdeSolver* _solver = nullptr;

	ODE(const size_t state_count = 1)
		: _x(state_count)
		, _x_dot(state_count)
	{
	}

	void set_state_count(const size_t state_count)
	{
		_x.resize(state_count);
		_x_dot.resize(state_count);
	}

	virtual ~ODE()
	{
	}

	// 设置求解器
	void set_solver(OdeSolver* solver)
	{
		delete _solver;
		_solver = solver;
	}

	// 设置龙格库塔法的常微分方程求解器
	bool set_rk_solver(const int solver_order);

public:
	virtual void step(const double dt, const double t)
	{
		_solver->integrate(this, dt, t, _x);
		do_output(t, _x, _x_dot);
	}

	//计算ODE的导数，仅依赖于给定的状态和时间，不依赖也不会改变内部状态
	//求解器求解过程中将调用此函数
	virtual array_d derivative(const double t, const array_d& x) = 0;

	// 输出
	virtual void do_output(const double t, const array_d& x, const array_d& x_d) = 0;

	// 获取状态变量
	const array_d& get_state() const
	{
		return _x;
	}

	// 设置状态变量
	void set_state(const array_d& x)
	{
		for (size_t i=0; i < std::min(x.size(), _x.size()); i++)
		{
			_x[i] = x[i];
		}
	}

	// 设置导数
	void set_dot(const array_d& x_dot)
	{
		_x_dot = x_dot;
	}

	// 获取导数
	const array_d& get_dot() const
	{
		return _x_dot;
	}
};
