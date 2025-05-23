
#include "block_trans_func.h"


std::string format_poly(const std::vector<double>& poly)
{
	fmt::memory_buffer buf_num;
	for (size_t i = poly.size() - 1; i > 0; i--)
	{
		if (i == 1)
			fmt::format_to(std::back_inserter(buf_num), "{}*S + ", poly[i]);
		else
			fmt::format_to(std::back_inserter(buf_num), "{}*S^{} + ", poly[i], i);
	}
	fmt::format_to(std::back_inserter(buf_num), "{}", poly[0]);
	return buf_num.data();
}


bool BlockTransFunc::init(const json& cfg)
{
	if (!Block::init(cfg))
	{
		loge("BlockTransFunc::init: Block::init failed!\n");
		return false;
	}
	// 加载并解析传递函数
	_num = read_json_double_vector(cfg, "num");
	_den = read_json_double_vector(cfg, "den");
	if (_num.empty() || _den.empty())
	{
		loge("BlockTransFunc[{}]: num or den is empty\n", _name);
		return false;
	}
	if (_num.size() > _den.size())
	{
		loge("BlockTransFunc[{}]: num size {} > den size {}\n", _name, _num.size(), _den.size());
		return false;
	}

	// 配置文件中，按照从高到低的顺序存储
	// 求解时，需要将其反转
	std::reverse(_num.begin(), _num.end());
	std::reverse(_den.begin(), _den.end());

	logi("BlockTransFunc[{}]:\n{}\n", _name, to_str());

	// 计算增益
	_k = _num.back() / _den.back();

	// 状态变量数量
	const int n = _den.size() - 1;
	set_state_count(n);

	// A矩阵的最后一行
	_a_last_row.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		_a_last_row[i] = -_den[i] / _den.back();
	}

	// C矩阵（矢量）
	_c.resize(n, 0.0);
	if (_num.size() == _den.size())
	{
		//C = | a[0] - b[0]			|
		//	  | a[1] - b[1]			|
		//	  | ...					|
		//	  | a[n - 1] - b[n - 1] |
		//D = [1]
		for (size_t i = 0; i < n; i++)
		{
			_c[i] = _num[i] / _num.back() - _den[i] / _den.back();
		}
	}
	else
	{
		//C =  | a[0]   |
		//     | a[1]   |
		//     | ...    |
		//     | a[m-1] |
		//     | 1      |
		//     | ...    |
		//     | 0      |
		for (size_t i = 0; i < _num.size(); i++)
		{
			_c[i] = _num[i] / _num.back();
		}
	}

	return true;
}


//用于输出传递函数
std::string BlockTransFunc::to_str() const
{
	auto num = format_poly(_num);
	auto den = format_poly(_den);

	fmt::memory_buffer buf;
	auto len = std::max(num.size(), den.size());
	fmt::format_to(std::back_inserter(buf), "{:^{}}\n", num, len);
	for (size_t i = 0; i < len; i++)
		fmt::format_to(std::back_inserter(buf), "-");
	fmt::format_to(std::back_inserter(buf), "\n");
	fmt::format_to(std::back_inserter(buf), "{:^{}}\n", den, len);

	return buf.data();
}


//计算ODE的导数，仅依赖于给定的状态和时间，不依赖也不会改变内部状态
//求解器求解过程中将调用此函数
// dot(x) = A x + B u
//A = |  0        1        0        ...    0      |         B =  | 0   |
//	  |  0        0        1        ...    0      |              | 0   |
//	  |  0        0        0        ...    0      |              | 0   |
//	  |  ...      ...      ...      ...    ...    |              | ... |
//	  |  0        0        0        ...    1      |              | 0   |
//	  | -b[0]    -b[1]    -b[2]     ...   -b[n-1] |              | 1   |
array_d BlockTransFunc::derivative(const double t, const array_d& x)
{
	if (x.size() == 0)
	{
		return {};
	}
	// dot(x)[0] = x[1]
	// dot(x)[1] = x[2]
	// ...
	// dot(x)[n-2] = x[n-1]
	auto x_d = x.shift(1);
	// dot(x)[n-1] = u - b . x
	x_d[x_d.size() - 1] = *_input[0] + (_a_last_row * x).sum();
	return x_d;
}


// 输出
// y = C x + D u
void BlockTransFunc::do_output(const double t, const array_d& x, const array_d& x_d)
{
	const double u = *_input[0];
	double& y = _output[0];
	// [4] / [5]
	if (x.size() == 0)
	{
		y = u * _k;
		return;
	}
	if (_num.size() == _den.size())
	{
		y = u + (x * _c).sum();
	}
	else
	{
		y = (x * _c).sum();
	}
	y *= _k;
}
