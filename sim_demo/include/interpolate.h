#pragma once
#include <vector>

using std::vector;


//二分法定位
inline
size_t bisect_array(double* xx, size_t n, double x)
{
	size_t i = 0;
	size_t j = n - 1;
	size_t k = 0;
	if (x <= xx[0])
	{
		return 0;
	}
	if (x > xx[j - 1])
	{
		return j - 1;
	}
	while (i < j)
	{
		k = (i + j) / 2;
		if (xx[k] < x)
		{
			i = k + 1;
		}
		else
		{
			j = k;
		}
	}
	return i - 1;
}


inline
int update_last_index(vector<double>& xx, double x, int& last_interval)
{
	const int n = static_cast<int>(xx.size());
	if (n <= 2) {
		last_interval = 0; return last_interval;
	}
	if (x < xx[0]) {
		last_interval = 0; return last_interval;
	}
	if (x > xx[n - 2]) {
		last_interval = n - 2; return last_interval;
	}

	last_interval = static_cast<int>(bisect_array(xx.data(), n, x));
	return last_interval;
}


class Interpolator
{
public:
	virtual ~Interpolator() = default;
};


class Interpolator1D : public Interpolator
{
public:
	vector<double> _x;
	vector<double> _y;
	int _last_index = 0;

	virtual double eval(double x) = 0;
};


class InterpolatorLinear : public Interpolator1D
{
protected:
	vector<double> _k;
public:
	InterpolatorLinear(const vector<double>& x, const vector<double>& y)
	{
		_x = x;
		_y = y;
		for (int i = 0; i < x.size() - 1; ++i)
		{
			_k.push_back((y[i + 1] - y[i]) / (x[i + 1] - x[i]));
		}
	}

	double eval(double x) override
	{
		if (x <= _x[0]) return _y[0];
		if (x >= _x.back()) return _y.back();
		auto idx = update_last_index(_x, x, _last_index);
		return _y[idx] + _k[idx] * (x - _x[idx]);
	}
};


class Interpolator2D : public Interpolator
{
public:
	vector<double> _x0;
	vector<double> _x1;
	vector<vector<double>> _y;
	int _last_index0 = 0;
	int _last_index1 = 0;

	virtual double eval(double x0, double x1) = 0;
};


class Interpolator2DLinear : public Interpolator2D
{
public:
	vector<vector<double>> _k0;
	vector<vector<double>> _k1;
	vector<vector<double>> _k01;

	Interpolator2DLinear(const vector<double>& x0, const vector<double>& x1, const vector<vector<double>>& y)
	{
		_x0 = x0;
		_x1 = x1;
		_y = y;
		
		linear_k(x0, y, 0, _k0);
		linear_k(x1, y, 1, _k1);
		linear_k(x1, _k0, 1, _k01);
	}

	double eval(double x0, double x1) override
	{
		if (x0 < _x0[0])
		{
			auto j = update_last_index(_x1, x1, _last_index1);
			if (x1 < _x1[0]) return _y[0][0];
			else if (x1 > _x1.back()) return _y.back()[0];
			else return _y[j][0] + _k0[j][0] * (x1 - _x1[j]);
		}
		else if (x0 > _x0.back())
		{
			auto j = update_last_index(_x1, x1, _last_index1);
			if (x1 < _x1[0]) return _y[0].back();
			else if (x1 > _x1.back()) return _y.back().back();
			else return _y[j].back() + _k0[j].back() * (x1 - _x1[j]);
		}

		if (x1 < _x1[0])
		{
			auto i = update_last_index(_x0, x0, _last_index0);
			if (x0 < _x0[0]) return _y[0][0];
			else if (x0 > _x0.back()) return _y.back()[0];
			else return _y[0][i] + _k0[0][i] * (x0 - _x0[i]);
		}
		else if (x1 > _x1.back())
		{
			auto i = update_last_index(_x0, x0, _last_index0);
			if (x0 < _x0[0]) return _y[0].back();
			else if (x0 > _x0.back()) return _y.back().back();
			else return _y.back()[i] + _k1.back()[i] * (x0 - _x0[i]);
		}
		auto i = update_last_index(_x0, x0, _last_index0);
		auto j = update_last_index(_x1, x1, _last_index1);
		const auto dx0 = (x0 - _x0[i]);
		const auto dx1 = (x1 - _x1[j]);
		return _y[j][i] + _k0[j][i] * dx0 + _k1[j][i] * dx1 + _k01[j][i] * dx0 * dx1;
	}

	void linear_k(const std::vector<double>& a, const std::vector<std::vector<double>>& f, const size_t dim, std::vector<std::vector<double>>& k)
	{
		k.resize(f.size());
		if (dim == 0)
		{
			for (size_t j = 0; j < f.size(); j++)
			{
				const auto n = a.size();
				k[j].resize(n);
				for (size_t i = 0; i < n - 1; i++)
				{
					k[j][i] = (f[j][i + 1] - f[j][i]) / (a[i + 1] - a[i]);
				}
				k[j][n - 1] = k[j][n - 2];
			}
		}
		else
		{
			const auto n = a.size();
			for (size_t j = 0; j < n - 1; j++)
			{
				k[j].resize(f[j].size());
				for (size_t i = 0; i < f[j].size(); i++)
				{
					k[j][i] = (f[j + 1][i] - f[j][i]) / (a[j + 1] - a[j]);
				}
			}
			k[n - 1] = k[n - 2];
		}
	}

};


