#pragma once
#include "block_ode.h"


//k/(TS+1)
class BlockLagFilter : public BlockOde
{
public:
	BlockLagFilter()
	{
		_type = "BlockLagFilter";
	}

	bool init(const json& cfg) override
	{
		_k = read_json_double(cfg, "k", 1.0);
		_T = read_json_double(cfg, "T", 1.0);
		return Block::init(cfg);
	}

	array_d derivative(const double t, const array_d& x) override
	{
		return { (*_input[0] * _k - x[0]) / _T};
	}

	void do_output(const double t, const array_d& x, const array_d& x_d) override
	{
		_output[0] = x[0];
	}

protected:
	double _k = 1.0;
	double _T = 1.0;
};
