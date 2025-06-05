#include "sim_engine.h"

#include <fstream>

#include "aircraft.h"
#include "sub_system.h"

#include "util.h"


SimEngine::~SimEngine()
{
	for (auto* vehicle : _vehicles)
	{
		for (auto* ss : vehicle->_sub_systems)
		{
			auto it = _dll_wrappers.find(ss->_type);
			if (it != _dll_wrappers.end())
			{
				it->second._func_destroy(ss);
			}
			else
			{
				loge("SimEngine::~SimEngine: failed to find dll for sub system: {}\n", ss->_class_name);
			}
		}
		delete vehicle;
	}

	for (const auto& [type, wrapper] : _dll_wrappers)
	{
		if (wrapper._dll)
		{
			FreeLibrary(wrapper._dll);
		}
	}
}


bool SimEngine::parse_mission(const std::string& filename)
{
	std::ifstream f(filename);
	json data;
	try
	{
		data = json::parse(f);
	}
	catch (json::parse_error& e)
	{
		loge("Parse error: {}", e.what());
		return false;
	}

	if (read_json_string(data, "type") != "mission")
	{
		loge("Parse error: not a mission file: {}", filename);
		return false;
	}

	_time_step = read_json_double(data, "time_step", 0.01);
	_max_time = read_json_double(data, "max_time", 10.0);

	int id = 0;
	for (auto& vehicle_config : data["vehicles"])
	{
		auto* vehicle = create_vehicle(vehicle_config, id);
		if (vehicle)
		{
			_vehicles.push_back(vehicle);
		}
		else
		{
			loge("failed to create vehicle: [{}]\n", id);
			return false;
		}
		id++;
	}

	return true;
}


Vehicle* SimEngine::create_vehicle(const json& vehicle_config, const int id)
{
	auto type = vehicle_config["type"].get<std::string>();

	Vehicle* vehicle = nullptr;
	if (type == "aircraft")
	{
		vehicle = new Aircraft();
	}
	else
	{
		loge("Unknown vehicle type: {}", type);
		return nullptr;
	}

	vehicle->_id = id;

	if (!vehicle->init(vehicle_config))
	{
		loge("Vehicle init failed: {}", type);
		delete vehicle;
		return nullptr;
	}

	int ss_id = 0;
	for (auto& sub_system_config : vehicle_config["sub_systems"])
	{
		auto* sub_system = create_sub_system(vehicle, vehicle_config, sub_system_config, ss_id++);
		if (sub_system)
		{
			vehicle->_sub_systems.push_back(sub_system);
		}
		else
		{
			loge("failed to create sub system\n");
			delete vehicle;
			return nullptr;
		}
	}
	// 绑定数据
	for (auto* sub_system : vehicle->_sub_systems)
	{
		if (!sub_system->bind_data())
		{
			loge("failed to bind data for sub system: {}[{}-{}]\n", 
				sub_system->_class_name, vehicle->_id, sub_system->_id);
			delete vehicle;
			return nullptr;
		}
	}

	return vehicle;
}


SubSystem* SimEngine::create_sub_system(Vehicle* parent, const json& vehicle_config, const json& sub_system_config, const int id)
{
	auto type = read_json_string(sub_system_config, "type");
	if (type.empty())
	{
		loge("SimEngine::create_sub_system: SubSystem type is empty.\n");
		return nullptr;
	}

	auto it = _dll_wrappers.find(type);
	if (it == _dll_wrappers.end())
	{
		if (!load_dll(type))
		{
			loge("SimEngine::create_sub_system: failed to load dll for type: {}\n", type);
			return nullptr;
		}
		it = _dll_wrappers.find(type);
	}

	SubSystem* sub_system = it->second._func_create();
	if (!sub_system)
	{
		loge("SimEngine::create_sub_system: failed to create sub system: {}\n", type);
		return nullptr;
	}

	sub_system->_type = type;
	sub_system->_id = id;
	sub_system->_vehicle = parent;
	if (!sub_system->init(vehicle_config, sub_system_config))
	{
		loge("Sub system init failed: {}\n", type);
		delete sub_system;
		return nullptr;
	}

	return sub_system;
}


bool SimEngine::load_dll(const std::string& type)
{
	SubSystemDllWrapper wrapper;

	// 拼接DLL名称
	std::string dll_name = std::string("sub_system\\ss_") + type;
#ifdef _DEBUG
	dll_name += "d";
#endif
	dll_name += ".dll";

	// 加载DLL
	wrapper._dll = LoadLibraryA(dll_name.c_str());
	if (!wrapper._dll)
	{
		loge("SimEngine::load_dll: failed to load \"{}\"\n", dll_name);
		return false;
	}

	// 读取函数create_sub_system
	wrapper._func_create = reinterpret_cast<func_create_sub_system>(GetProcAddress(wrapper._dll, "create_sub_system"));
	if (!wrapper._func_create)
	{
		loge("SimEngine::load_dll: failed to get function \"create_sub_system\" in \"{}\"", dll_name);
		FreeLibrary(wrapper._dll);
		return false;
	}

	// 读取函数destroy_sub_system
	wrapper._func_destroy = reinterpret_cast<func_destroy_sub_system>(GetProcAddress(wrapper._dll, "destroy_sub_system"));
	if (!wrapper._func_destroy)
	{
		loge("SimEngine::load_dll: failed to get function \"destroy_sub_system\" in \"{}\"", dll_name);
		FreeLibrary(wrapper._dll);
		return false;
	}

	_dll_wrappers[type] = wrapper;
	return true;
}


bool SimEngine::step()
{
	_time += _time_step;
	if (_time > _max_time)
	{
		return false;
	}
	logi("SimEngine::step: {:.3f}\n", _time);
	auto t0 = std::chrono::high_resolution_clock::now();
	fmt::memory_buffer buf;
	for (auto* vehicle : _vehicles)
	{
		vehicle->step(_time_step, _time);
		const auto t1 = std::chrono::high_resolution_clock::now();
		const auto dt = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
		fmt::format_to(std::back_inserter(buf), "[{}]: {:<20} {:.3f}ms\n",
			vehicle->_id, vehicle->_class_name, static_cast<double>(dt.count()) / 1000.0);
		t0 = t1;
	}
	logi("SimEngine::step: vehicle step() time [t={:.3f}]: \n{}", _time, fmt::to_string(buf));
	return true;
}


void SimEngine::start()
{
	_interval = std::chrono::milliseconds(static_cast<int>(_time_step * 1000));
	_thread = std::thread([this]() { run(); });
}


void SimEngine::run()
{
	auto last_start = std::chrono::steady_clock::now();
	while (!_stop)
	{
		auto start = std::chrono::steady_clock::now();
		double delta_ms = std::chrono::duration<double, std::milli>(start - last_start).count();
		if (abs(_time_step * 1000 - delta_ms) > 1.0e-3)
		{
			logw("SimEngine::run: time step mismatch: expected {:.3f}ms, got {:.3f}ms\n", _time_step * 1000, delta_ms);
		}
		last_start = start;

		// 仿真时间推进
		if (!step())
		{
			break;
		}

		// 更新输出的状态数据
		update_vehicle_state();

		if (abs(_time - round(_time)) < 1.0e-6)
		{
			fmt::print("sim time: {:.3f}\n", _time);
		}
		while (std::chrono::steady_clock::now() < start + _interval)
		{
			// 空循环（占用 CPU）
		}
	}
	_finished = true;
	logi("SimEngine::run: simulation ended at t={:.3f}\n", _time);
}


void SimEngine::join()
{
	if (_thread.joinable())
	{
		_thread.join();
	}
	else
	{
		logw("SimEngine::join: thread not joinable\n");
	}
}


bool SimEngine::bind_vehicle_state(const std::vector<std::vector<std::string>>& names)
{
	_vehicle_states.resize(_vehicles.size());
	for (size_t i = 0; i < _vehicles.size(); i++)
	{
		_vehicle_states[i].resize(names[i].size());
	}

	for (size_t i = 0; i < _vehicles.size(); i++)
	{
		if (i >= names.size())
		{
			loge("bind_vehicle_state: vehicle index out of range: {}\n", i);
			return false;
		}
		int j = 0;
		for (const auto& name : names[i])
		{
			if (!_vehicle_states_binder.bind(_vehicles[i]->_data_pool, name, _vehicle_states[i][j]))
			{
				loge("bind_vehicle_state: failed to bind {} for vehicle {}\n", name, i);
				return false;
			}
			j++;
		}
	}
	return true;
}


void SimEngine::update_vehicle_state()
{
	std::lock_guard<std::mutex> lock(_mutex);
	_vehicle_states_binder.update();
}


std::vector<std::valarray<double>> SimEngine::get_vehicle_states() const
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _vehicle_states;
}
