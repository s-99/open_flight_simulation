#pragma once

#include "ode.h"

class OdeSolverRungeKuttaN : public OdeSolver
{
public:
	OdeSolverRungeKuttaN(const int order);

	void integrate(ODE* ode, const double dt, const double t, array_d& x) override;

	double get_a(const size_t i, const size_t j) const
	{
		return a[(_stage - 1)*i + j];
	}

	int _order = 1;
	int _stage = 1;
	const double *a = nullptr;
	const double *b = nullptr;
	const double *c = nullptr;
};