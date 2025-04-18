#pragma once

#include "data_recorder.h"
#include "sub_system.h"
#include "vehicle.h"
#include "vec3.h"
#include "quat.h"
#include "matrix3x3.h"


class Dynamic6DOF : public SubSystem
{
public:
	Dynamic6DOF()
	{
		_class_name = "Dynamic6DOF";
	}
	virtual ~Dynamic6DOF() = default;

	bool bind_data() override;
	bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

	void reg_data(const std::string& name, double& data)
	{
		_vehicle->_data_pool.reg_data(name, data, "Dynamic6DOF");
		_recorder.reg(name, data);
	}

	// inputs
	Vec3 _Fa;
	Vec3 _Ma;
	Vec3 _Fe;
	Vec3 _Me;

	// outputs
	Vec3 _V;
	Vec3 _Omega;
	Vec3 _pos;
	Vec3 _Va;
	Vec3 _euler;
	Quaternion _quat;
	double _Vel;
	double _alpha;
	double _beta;
	double _alpha_dot;
	double _q_bar;
	double _mach;
	double _h;

	// data
	Matrix3x3 _I;
	double _mass;

	DataRecorder _recorder;

	DataBinder _binder;
};
