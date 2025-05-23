#include "sub_system_engine.h"

#include <json.hpp>
#include <fstream>

#include "util.h"


DLL_EXPORT
SubSystem* create_sub_system()
{
	return new SubSystemEngine();
}


DLL_EXPORT
void destroy_sub_system(SubSystem* ss)
{
	delete ss;
}


bool EngineTable::parse(const string& name, const json& content)
{
	if (!Table::parse(name, content))
	{
		return false;
	}

	if (_dim == 2)
	{
		_interpolator = new Interpolator2DLinear(_x0, _x1, _value);
	}
	else if (_dim == 1)
	{
		_interpolator = new InterpolatorLinear(_x0, _value[0]);
	}
	else
	{
		loge("EngineTable::parse: invalid engine dim: {}\n", _dim);
		return false;
	}

	return true;
}


double EngineTable::eval() const
{
	if (_dim == 2)
	{
		return dynamic_cast<Interpolator2DLinear*>(_interpolator)->eval(_x0_value, _x1_value);
	}
	// else if(_dim == 1)
	return dynamic_cast<InterpolatorLinear*>(_interpolator)->eval(_x0_value);
}


void SubSystemEngine::step(double dt, double t)
{
	// 读取输入
	_binder.update();

	// 滞后特性仿真
	_lag_filter.step(dt, t);
	_n = _lag_filter.get_output(0);

	// 计算推力
	if (_dim == 2)
	{
		_thrust = _tables[0].eval();
		if (_tables[0]._x0_name != "n" && _tables[0]._x1_name != "n")
		{
			_thrust *= _n;
		}
	}
	else if (_dim == 3)
	{
		std::vector<double> T(_n_table.size());
		for (size_t i = 0; i < _tables.size(); i++)
		{
			T[i] = _tables[i].eval();
		}
		_thrust = interp1_linear(_n_table, T, _n);
	}

	_thrust *= _engine_count;

	// 记录数据
	_recorder.update(t);
}


bool SubSystemEngine::init(const json& vehicle_config, const json& sub_system_config)
{
	logi("SubSystemEngine::init\n");

	auto& eng = _vehicle->_data_file["engine"];
	_dim = read_json_int(eng, "dim", 2);
	if (_dim <= 2)
	{
		// 维数<=2时，加载一个表格
		_tables.resize(1);
		if (!_tables[0].parse("T", eng["T"]))
		{
			loge("SubSystemEngine::init: parse engine table failed\n");
			return false;
		}
	}
	else if (_dim == 3)
	{
		// 维数为3时，加载全部T开头的节点
		for (auto& [key, value] : eng.items())
		{
			if (key.empty()) continue;
			if (key[0] != 'T') continue;
			auto& table = _tables.emplace_back();
			if (!table.parse(key, value))
			{
				loge("SubSystemEngine::init: parse engine table failed\n");
				return false;
			}
			_n_table.push_back(strtod(&key[1], nullptr));
		}
	}
	else
	{
		loge("SubSystemEngine::init: invalid engine dim: {}\n", _dim);
		return false;
	}

	// 滞后特性初始化
	_lag_filter.init(1.0, read_json_double(eng, "lag", 0.0));
	_lag_filter.set_rk_solver(4);
	_lag_filter.link_to(&_throttle);

	_engine_count = read_json_int(eng, "count", 1);

	// 导出数据
	reg_data("n", _n);
	reg_data("thrust", _thrust);

	return true;
}


bool SubSystemEngine::bind_data()
{
	std::string failed_input;
	// 绑定数据
	for (auto& table : _tables)
	{
		if (!_binder.bind(_vehicle->_data_pool, table._x0_name, table._x0_value))
		{
			failed_input += (table._x0_name + ",");
		}
		if (!_binder.bind(_vehicle->_data_pool, table._x1_name, table._x1_value))
		{
			failed_input += (table._x1_name + ",");
		}
	}

	if (!failed_input.empty())
	{
		loge("SubSystemEngine::bind_data: {}[{}-{}] bind {} failed.\n", _class_name, _vehicle->_id, _id, failed_input);
		return false;
	}

	BIND_DATA(throttle);
	_recorder.update(0);
	return true;
}
