#pragma once

#include "sub_system.h"
#include <cmath>

const double PI = 3.14159265358979323846;

struct Sig
{
    virtual ~Sig() = default;
    virtual void update_value(const double t, const double dt) = 0;

    double _value = 0.0;
};

struct SigConst : public Sig
{
    void update_value(const double, const double) override
    {
    }
};

struct SigStep : public Sig
{
    double _t_start = 1.0;
    double _amp = 1.0;
    void update_value(const double t, const double) override
    {
        _value = t >= _t_start ? _amp : 0.0;
    }
};

struct SigPulse : public Sig
{
    double _t_start = 1.0;
    double _amp = 1.0;
    void update_value(const double t, const double dt) override
    {
        _value = abs(t - _t_start) < (dt / 2.0) ? _amp : 0.0;
    }
};

struct SigSin : public Sig
{
    double _t_start = 1.0;
    double _amp = 1.0;
    double _freq = 1.0;
    int    _freq_unit = 0;  // 0: Hz, 1: rad/s
    void update_value(const double t, const double) override
    {
        _value = t > _t_start ? _amp * sin((t - _t_start) * (_freq_unit == 0 ? 2.0 * PI : 1.0) * _freq) : 0.0;
    }
};

class SigGenerator : public SubSystem
{
public:
	SigGenerator()
	{
		_class_name = "SigGenerator";
	}
	~SigGenerator() override
	{
		for (auto* builder : _sig_builders)
		{
			delete builder;
		}
	}

	bool bind_data() override { return true; }
    bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

    std::vector<Sig*> _sig_builders;
};
