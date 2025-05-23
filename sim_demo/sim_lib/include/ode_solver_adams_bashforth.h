#pragma once

#include <deque>

#include "ode.h"

class OdeSolverAdamsBashforthN : public OdeSolver
{
public:
	OdeSolverAdamsBashforthN(const int order);

	void integrate(ODE* ode, const double dt, const double t, array_d& x) override;

	int _order = 1;
	std::deque<array_d> _dot_history;
};