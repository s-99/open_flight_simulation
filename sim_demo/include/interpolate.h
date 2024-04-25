#pragma once
#include <vector>

using std::vector;


inline
int update_last_index(vector<double>& xx, double x, int& last_interval)
{
	const size_t n = xx.size();
	if (n <= 2) {
		last_interval = 0; return last_interval;
	}

	const auto x_left = xx.begin() + last_interval;
	if (*(x_left + 1) <= x) {
		if (x >= xx[n - 2])  last_interval = static_cast<int>(n - 2); // last interval
		else if (x < *(x_left + 2)) ++last_interval; // next interval
		else { // search
			const auto xe = xx.end();
			last_interval += static_cast<int>(std::lower_bound(x_left, xe, x) - x_left);
			if (xx[last_interval] > x) --last_interval;
		}
	}
	else if (x < *x_left) {
		if (x < xx[1]) last_interval = 0; // first interval
		else if (*(x_left - 1) <= x) --last_interval; // previous interval
		else { // search
			last_interval = static_cast<int>(std::lower_bound(xx.begin(), x_left, x) - xx.begin());
			if (xx[last_interval] > x) --last_interval;
		}
	}
	else {
		// x_left <= x < x_left + 1
	}
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


