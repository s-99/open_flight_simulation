#pragma once

#include "block.h"

#include <queue>
#include <string>
#include <algorithm>

#include "util.h"

//求和环节
class BlockSum : public Block
{
public:
	BlockSum()
	{
		_type = "BlockSum";
	}

	bool init(const json& cfg) override
	{
		const auto sign = read_json_string(cfg, "sign");
		for (auto& s : sign)
		{
			if (s == '+')
			{
				_sign.push_back(1.0);
			}
			else if (s == '-')
			{
				_sign.push_back(-1.0);
			}
			else
			{
				loge("BlockSum: invalid sign '{}'\n", s);
				return false;
			}
		}
		_input.resize(_sign.size(), nullptr);
		return Block::init(cfg);
	}

	void step(const double dt, const double t) override
	{
		_output[0] = 0;
		for (size_t i = 0; i < _input.size(); i++)
		{
			_output[0] += _sign[i] * (*_input[i]);
		}
	}

protected:
	std::vector<double> _sign;
};


//增益环节
class BlockGain : public Block
{
public:
	BlockGain()
	{
		_type = "BlockGain";
	}

	bool init(const json& cfg) override
	{
		_k = read_json_double(cfg, "k", 1.0);
		return Block::init(cfg);
	}

	void step(const double dt, const double t) override
	{
		_output[0] = *_input[0] * _k;
	}

protected:
	double _k = 1.0;
};


//求导环节
class BlockDerivative : public Block
{
public:
	BlockDerivative()
	{
		_type = "BlockDerivative";
	}

	void step(const double dt, const double t) override
	{
		if (_first)
		{
			_output[0] = 0.0;
			_first = false;
		}
		else
		{
			_output[0] = (*_input[0] - _last) / dt;
		}

		_last = *_input[0];
	}

protected:
	double	_last = 0.0;
	bool	_first = true;
};


//饱和(限幅)环节
class BlockSaturation : public Block
{
public:
	BlockSaturation()
	{
		_type = "BlockSaturation";
	}

	bool init(const json& cfg) override
	{
		_lower = read_json_double(cfg, "lower", -1.0);
		_upper = read_json_double(cfg, "upper", 1.0);
		return Block::init(cfg);
	}

	void step(const double dt, const double t) override
	{
		_output[0] = std::clamp(*_input[0], _lower, _upper);
	}

protected:
	double _lower = -1;
	double _upper = 1;
};


//速率限制器
class BlockRateLimiter : public Block
{
public:
	BlockRateLimiter()
	{
		_type = "BlockRateLimiter";
	}

	bool init(const json& cfg) override
	{
		_lower = read_json_double(cfg, "lower", -1.0);
		_upper = read_json_double(cfg, "upper", 1.0);
		return Block::init(cfg);
	}

	void step(const double dt, const double t) override
	{
		double rate = (*_input[0] - _last) / dt;
		rate = std::clamp(rate, _lower, _upper);
		_last += rate * dt;
		_output[0] = _last;
	}

protected:
	double _last = 0;
	double _upper = 1.0;
	double _lower = -1.0;
};


//延迟环节，e^(-tau*s)
class BlockDelay : public Block
{
public:
	BlockDelay()
	{
		_type = "BlockDelay";
	}

	bool init(const json& cfg) override
	{
		_tau = read_json_double(cfg, "tau", 1.0);
		return Block::init(cfg);
	}

	void step(const double dt, const double t) override
	{
		_delay_q.emplace(t, *_input[0]);
		while (t - _delay_q.front().first > _tau)
		{
			_output[0] = _delay_q.front().second;
			_delay_q.pop();
		}
	}

protected:
	// 延迟队列，存储时间和输入值
	std::queue<std::pair<double, double>> _delay_q;
	double _tau = 1.0;
};

