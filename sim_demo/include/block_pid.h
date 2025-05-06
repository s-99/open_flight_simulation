#pragma once
#include "block_ode.h"


//PID环节
class BlockPid : public BlockOde
{
public:
	BlockPid()
	{
		_type = "BlockPid";
	}

	bool init(const json& cfg) override
	{
		_kp = read_json_double(cfg, "kp", 1.0);
		_ki = read_json_double(cfg, "ki", 0.0);
		_kd = read_json_double(cfg, "kd", 0.0);
		_clamp_i = read_json_double(cfg, "clamp_i", -1.0);
		return Block::init(cfg);
	}

	array_d derivative(const double t, const array_d& x) override
	{
		return { *_input[0] };
	}

	void do_output(const double t, const array_d& x, const array_d& x_d) override
	{
		const auto u = *_input[0];
		const double d = _first ? 0.0 : ((u - _last_input) / t);
		_last_input = u;
		auto i = _ki * x[0];
		if (_clamp_i > 0)
		{
			i = std::clamp(i, -_clamp_i, _clamp_i);
		}
		_output[0] = _kp * u + i + _kd * d;
	}

protected:
	double _kp = 1.0;		//比例系数
	double _ki = 0.0;		//积分系数
	double _kd = 0.0;		//微分系数
	double _clamp_i = 1.0;	//积分器限幅
	double _last_input = 0.0;	//上一步的input
	bool _first = true;		//第一步
};
