#include "block.h"

#include "block_ode.h"


BlockSubSystem::~BlockSubSystem()
{
	for (auto* block : _blocks)
	{
		delete block;
	}
	_blocks.clear();
	_input_sig.clear();
	_input.clear();
}


Block* BlockSubSystem::find_block(const std::string& name) const
{
	for (auto* block : _blocks)
	{
		if (block->_name == name)
		{
			return block;
		}
	}
	return nullptr;
}


double* BlockSubSystem::find_signal(const std::string& name, const int output_idx)
{
	for (auto& input : _input_sig)
	{
		if (input._name == name)
		{
			return &input._value;
		}
	}
	auto* block = find_block(name);
	if (block != nullptr)
	{
		return &block->get_output(output_idx);
	}
	return nullptr;
}


bool BlockSubSystem::init(const json& cfg)
{
	// 输入端口
	auto input = cfg.find("input");
	_input.clear();
	if (input != cfg.end())
	{
		for (auto& i : *input)
		{
			auto name = i.get<std::string>();
			auto& sig = _input_sig.emplace_back(name);
			_input.push_back(&sig._value);
		}
	}

	// 所有模块
	auto blocks = cfg.find("blocks");
	if (blocks != cfg.end())
	{
		for (auto& b : *blocks)
		{
			if (!b.is_object())
			{
				continue;
			}

			auto name = read_json_string(b, "name");
			// 检查模块名称是否重复
			if (find_block(name) != nullptr)
			{
				loge("block {} already exists\n", name);
				return false;
			}

			// 创建模块
			auto type = read_json_string(b, "type");
			auto* block = create_block(type);
			if (block == nullptr)
			{
				loge("create block {}:{} failed\n", name, type);
				return false;
			}

			// 初始化模块
			if (!block->init(b))
			{
				loge("block {}:{} init failed\n", name, type);
				delete block;
				return false;
			}
			_blocks.push_back(block);
		}
	}

	// 连接
	auto connections = cfg.find("connections");
	if (connections != cfg.end())
	{
		int i = 0;
		for (auto& c : *connections)
		{
			// 源
			auto from = read_json_string(c, "from");
			int from_idx = read_json_int(c, "from_idx", 0);
			double* signal = find_signal(from, from_idx);
			if (!signal)
			{
				loge("connection[{}]: signal {}:{} not found\n", i, from, from_idx);
				return false;
			}

			// 目标
			auto to = read_json_string(c, "to");
			int to_idx = read_json_int(c, "to_idx", 0);
			auto* to_block = find_block(to);
			if (!to_block)
			{
				loge("connection[{}]: block {} or {} not found\n", i, from, to);
				return false;
			}

			// 连接
			to_block->link_to(signal, to_idx);

			i++;
		}
	}

	// 输出
	auto output = cfg.find("output");
	if (output != cfg.end())
	{
		for (auto& o : *output)
		{
			auto name = read_json_string(o, "name");
			auto block_name = read_json_string(o, "block");
			auto* block = find_block(block_name);
			if (!block)
			{
				loge("output {} not found\n", name);
				return false;
			}
			int idx = read_json_int(o, "port_idx", 0);
			if (idx < 0 || idx >= block->_output.size())
			{
				loge("output {}:{} port index {} out of range\n", block_name, name, idx);
				return false;
			}
			_output_sig.emplace_back(name, block, idx);
		}
	}
	_output.resize(_output_sig.size(), 0.0);

	return Block::init(cfg);
}


void BlockSubSystem::set_solver(OdeSolver* solver)
{
	for (auto* block : _blocks)
	{
		if (auto* block_ode = dynamic_cast<BlockOde*>(block))
		{
			block_ode->set_solver(solver);
		}
	}
}


void BlockSubSystem::step(const double dt, const double t)
{
	for (auto* block : _blocks)
	{
		block->step(dt, t);
	}

	for (size_t i = 0; i < _output_sig.size(); i++)
	{
		_output[i] = _output_sig[i]._block->get_output(_output_sig[i]._port_idx);
	}
}
