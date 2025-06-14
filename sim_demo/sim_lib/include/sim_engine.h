﻿#pragma once
#include <windows.h>

#include "vehicle.h"

#include <string>
#include <mutex>


typedef SubSystem* (*func_create_sub_system)();
typedef void (*func_destroy_sub_system)(SubSystem*);


struct SubSystemDllWrapper
{
	HMODULE _dll = nullptr;
	func_create_sub_system _func_create = nullptr;
	func_destroy_sub_system _func_destroy = nullptr;
};


// 仿真引擎，用于管理仿真对象、驱动仿真运行
class SimEngine
{
public:
	SimEngine() = default;
	virtual ~SimEngine();

	bool parse_mission(const std::string& filename);

	SubSystem* create_sub_system(Vehicle* parent, const json& vehicle_config, const json& sub_system_config, const int id);
	Vehicle* create_vehicle(const json& vehicle_config, const int id);

	bool step();

	bool load_dll(const std::string& type);

	std::map<std::string, SubSystemDllWrapper> _dll_wrappers;

	std::vector<Vehicle*> _vehicles;

	double _time = 0.0;
	double _time_step = 0.01;
	double _max_time = 0.0;

	// ========================================================================
	// 独立仿真线程相关代码

	// 启动独立仿真线程
	void start();

	// 仿真线程函数
	void run();

	// 等待仿真线程结束
	void join();

	// 停止仿真线程
	void stop() { _stop = true; }

	// 仿真时间间隔，单位毫秒
	std::chrono::milliseconds _interval;

	// 仿真线程
	std::thread _thread;

	// 停止标志
	bool _stop = false;

	// 仿真是否已完成
	bool _finished = false;

	// ========================================================================
	// 新增：可由外部访问的数据

	// 每顶层仿真对象的状态信息
	std::vector<std::valarray<double>> _vehicle_states;
	DataBinder _vehicle_states_binder;

	// 用来保护_vehicle_states
	mutable std::mutex _mutex;

	// 绑定数据
	bool bind_vehicle_state(const std::vector<std::vector<std::string>>& names);

	// 更新绑定的数据
	void update_vehicle_state();

	// 读取绑定的数据
	std::vector<std::valarray<double>> get_vehicle_states() const;
};
