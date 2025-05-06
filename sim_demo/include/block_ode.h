#pragma once

#include "block.h"
#include "ode.h"

//基于ODE的连续环节
class BlockOde : public Block, public ODE
{
public:
	void step(const double dt, const double t) override
	{
		ODE::step(dt, t);
	}
};