#pragma once
#include "data_pool.h"
#include "data_recorder.h"
#include "sub_system.h"
#include "vehicle.h"
#include <windows.h>


class SubSystemJoystick: public SubSystem
{
public:
	SubSystemJoystick()
	{
		_class_name = "SubSystemJoystick";
	}
	~SubSystemJoystick() override = default;

	bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

	bool bind_data() override { return true; }

	void reg_data(const std::string& name, double& data)
	{
		_vehicle->_data_pool.reg_data(name, data, "SubSystemJoystick");
	}

	double _stick_right = 0.0f;		// 操纵杆x轴
	double _stick_push = 0.0f;		// 操纵杆y轴
	double _pedal_right = 0.0f;		// 脚蹬
	double _throttle = 0.0f;		// 操纵杆油门量

	double _trim_push = 0.0f;		// 操纵杆纵向配平量，推杆方向为正
	int _trim_push_button = -1;		// 操纵杆纵向配平量推杆按钮
	int _trim_pull_button = -1;		// 操纵杆纵向配平量拉杆按钮

	JOYCAPS _jc;  // joystick capabilities
};
