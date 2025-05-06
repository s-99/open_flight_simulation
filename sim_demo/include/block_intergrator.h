#pragma once
#include "block_ode.h"


//积分环节
class BlockIntegrator : public BlockOde
{
public:
	BlockIntegrator()
	{
		_type = "BlockIntegrator";
	}

	array_d derivative(const double t, const array_d& x) override
	{
		return { *_input[0] };
	}

	void do_output(const double t, const array_d& x, const array_d& x_d) override
	{
		_output[0] = x[0];
	}
};
