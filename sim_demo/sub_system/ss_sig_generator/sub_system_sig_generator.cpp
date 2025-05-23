#include "fmtlog.h"
#include "sub_system_sig_generator.h"
#include "util.h"
#include "vehicle.h"


DLL_EXPORT
SubSystem* create_sub_system()
{
	return new SubSystemSigGenerator();
}


DLL_EXPORT
void destroy_sub_system(SubSystem* ss)
{
	delete ss;
}


bool SubSystemSigGenerator::init(const json& vehicle_config, const json& sub_system_config)
{
	for (auto& sig_cfg : sub_system_config["sig"])
	{
		auto name = sig_cfg["sig_name"].get<std::string>();

		auto type = sig_cfg["sig_type"].get<std::string>();
		if (type == "const")
		{
			auto* builder = new SigConst();
			builder->_value = read_json_double(sig_cfg, "value", 0.0);
			_vehicle->_data_pool.reg_data(name, builder->_value, "SubSystemSigGenerator");
			_sig_builders.push_back(builder);
		}
		else if (type == "step")
		{
			auto* builder = new SigStep();
			builder->_t_start = read_json_double(sig_cfg, "t_start", 1.0);
			builder->_amp = read_json_double(sig_cfg, "amp", 1.0);
			_vehicle->_data_pool.reg_data(name, builder->_value, "SubSystemSigGenerator");
			_sig_builders.push_back(builder);
		}
		else if (type == "pulse")
		{
			auto* builder = new SigPulse();
			builder->_t_start = read_json_double(sig_cfg, "t_start", 1.0);
			builder->_amp = read_json_double(sig_cfg, "amp", 1.0);
			_vehicle->_data_pool.reg_data(name, builder->_value, "SubSystemSigGenerator");
			_sig_builders.push_back(builder);
		}
		else if (type == "sin")
		{
			auto* builder = new SigSin();
			builder->_t_start = read_json_double(sig_cfg, "t_start", 1.0);
			builder->_amp = read_json_double(sig_cfg, "amp", 1.0);
			builder->_freq = read_json_double(sig_cfg, "freq", 1.0);
			builder->_freq_unit = read_json_int(sig_cfg, "freq_unit", 1);
			_vehicle->_data_pool.reg_data(name, builder->_value, "SubSystemSigGenerator");
			_sig_builders.push_back(builder);
		}
		else
		{
			loge("Unknown sig type: {}", type);
			return false;
		}
	}
	return true;
}


void SubSystemSigGenerator::step(double dt, double t)
{
	for (auto* builder : _sig_builders)
	{
		builder->update_value(t, dt);
	}
}
