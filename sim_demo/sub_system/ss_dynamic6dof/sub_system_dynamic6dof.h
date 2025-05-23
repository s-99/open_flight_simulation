#pragma once

#include "data_recorder.h"
#include "sub_system.h"
#include "vehicle.h"
#include "vec3.h"
#include "quat.h"
#include "matrix3x3.h"
#include "ode.h"


class SubSystemDynamic6DOF : public SubSystem, public ODE
{
public:
	SubSystemDynamic6DOF()
		: ODE(13)
	{
		_class_name = "SubSystemDynamic6DOF";
	}
	~SubSystemDynamic6DOF() override;

	bool bind_data() override;
	bool init(const json& vehicle_config, const json& sub_system_config) override;
	void step(double dt, double t) override;

	void reg_data(const std::string& name, double& data)
	{
		_vehicle->_data_pool.reg_data(name, data, "SubSystemDynamic6DOF");
		_recorder.reg(name, data);
	}

	array_d derivative(const double t, const array_d& x) override;
	void do_output(const double t, const array_d& x, const array_d& x_d) override;


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
	Vec3 _Omega_s;
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
