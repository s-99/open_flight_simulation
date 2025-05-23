#pragma once

#include "data_pool.h"
#include "sub_system.h"
#include "block.h"
#include "data_recorder.h"


class SubSystemFcs : public SubSystem
{
public:
	SubSystemFcs()
	{
		_class_name = "SubSystemFcs";
	}
	~SubSystemFcs() override = default;

	bool bind_data() override;
	bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

	BlockSubSystem _fcs_model;

	DataRecorder _recorder;

	DataBinder _binder;
};
