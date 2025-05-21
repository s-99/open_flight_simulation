#pragma once
#include "data_pool.h"
#include "data_recorder.h"
#include "sub_system.h"
#include "vehicle.h"
#include "aero_model.h"
#include "block_lag_filter.h"


class EngineTable : public Table
{
public:
	Interpolator* _interpolator = nullptr;
	double _x0_value = 0;
	double _x1_value = 0;

	bool parse(const string& name, const json& content) override;
	double eval() const;
};


class Engine : public SubSystem
{
public:
	Engine()
	{
		_class_name = "Engine";
	}
	~Engine() override = default;

	bool bind_data() override;
	bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

	void reg_data(const std::string& name, double& data)
	{
		_vehicle->_data_pool.reg_data(name, data, "SubSystemEngine");
		_recorder.reg(name, data);
	}

	// dim<=2时，_tables[0]为表格，按照x0,x1的名称绑定输入进行插值
	// dim为3时，_tables[i]按照x0,x1的名称绑定输入进行插值，插值后，第三维按照_n进行插值
	int _dim = 2;
	std::vector<EngineTable> _tables;
	std::vector<double> _n_table;

	BlockLagFilter _lag_filter;

	double _throttle = 0.0;
	double _n = 0.0;
	double _thrust = 0.0;
	int _engine_count = 1;

	DataRecorder _recorder;

	DataBinder _binder;
};
