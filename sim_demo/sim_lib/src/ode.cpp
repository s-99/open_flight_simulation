#include "ode.h"

#include "ode_solver_runge_kutta.h"


bool ODE::set_rk_solver(const int solver_order)
{
	//设置求解器
	set_solver(new OdeSolverRungeKuttaN(solver_order));
	return true;
}
