﻿#pragma once

#include "aero_model.h"
#include "data_pool.h"
#include "sub_system.h"
#include "data_recorder.h"


class SubSystemAero : public SubSystem
{
public:
	SubSystemAero()
	{
		_class_name = "SubSystemAero";
	}
	~SubSystemAero() override = default;

	bool init(const json& vehicle_config, const json& sub_system_config) override;
	bool bind_data() override;
	void step(double dt, double t) override;

	AeroModel _model;

	DataRecorder _recorder;
	// inputs
	//double _de, _da, _dr, _df;

	// outputs
	double _Fx, _Fy, _Fz;
	double _L, _M, _N;

	DataBinder _binder;
};
