#pragma once
#include <json.hpp>
#include <vector>

#include "ode.h"
#include "util.h"

using std::vector;
using json = nlohmann::ordered_json;


// 飞控模块的基类
class Block
{
public:
	std::string _type;
	std::string _name;
	vector<double*> _input = {nullptr};
	vector<double> _output = {0.0};

	Block() = default;
	virtual ~Block() = default;

	virtual bool init(const json& cfg)
	{
		_name = read_json_string(cfg, "name");
		return true;
	}

	virtual void step(const double dt, const double t) = 0;

	void link_to(double* in, int input_idx = 0)
	{
		_input[input_idx] = in;
	}

	double& get_output(int i)
	{
		return _output[i];
	}
};


// 输入环节
class BlockSubSystemInput
{
public:
	BlockSubSystemInput(const std::string& name)
	{
		_name = name;
	}

	std::string _name;
	double _value = 0.0;
};


// 输出环节
class BlockSubSystemOutput
{
public:
	BlockSubSystemOutput(const std::string& name, Block* block, int port_idx)
	{
		_block = block;
		_name = name;
		_port_idx = port_idx;
	}

	std::string _name;
	Block* _block = nullptr;
	int _port_idx = 0;
};


// 飞控模块的子系统
class BlockSubSystem : public Block
{
public:
	BlockSubSystem()
	{
		_type = "BlockSubSystem";
	}

	~BlockSubSystem() override;

	bool init(const json& cfg) override;

	void step(const double dt, const double t) override;

	// 查找给定名称的模块
	Block* find_block(const std::string& name) const;

	// 查找BlockSubSystem的输入或者中间模块的输出，优先查找输入
	// name: 输入或者模块名称
	// output_idx: 模块的输出索引，如果是输入，则无效
	double* find_signal(const std::string& name, const int output_idx = 0);

	// 设置求解器，内部如果有BlockOde类型的模块，则必须设置求解器
	void set_solver(OdeSolver* solver);

	vector<Block*> _blocks;
	vector<BlockSubSystemInput> _input_sig;
	vector<BlockSubSystemOutput> _output_sig;
};


// 工厂函数，创建飞控模块
Block* create_block(const string& type);
