#pragma once

#include <string>
#include <vector>
#include <format>

using std::string;
using std::vector;


inline
bool starts_with(const string& str, const string& prefix)
{
	return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}


inline
vector<vector<double>> reshape(const vector<double>& v, size_t n, size_t m)
{
	vector<vector<double>> result(n, vector<double>(m));
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < m; j++)
		{
			result[i][j] = v[i * m + j];
		}
	}
	return result;
}


inline
string dump_vector(const vector<double>& v, int n)
{
	int i = 0;
	string result;
	for (auto& value : v)
	{
		result += std::format("{:8.3f} ", value);
		i++;
		if (i >= n)
		{
			if (v.size() > i) result += " ...";
			break;
		}
	}
	return result;
}



//有效地球半径,单位m
#define R_EARTH 6356.766e3
//海平面重力加速度,单位m/s^2
#define GRAVITY0 9.80665    


inline
auto cal_std_atm(const double h)
{
	double H = (R_EARTH * h) / (R_EARTH + h);
	double temperature, pressure, density, sound_speed;
	//根据不同海拔高度选择不同公式进行计算
	if (H <= 11000)
	{
		//288.15:标准海平面温度
		temperature = 288.15 - 0.0065 * H;
		pressure = 101325 * pow(1 - 0.225577e-4 * H, 5.25588);
		density = 1.225 * pow(1 - 0.225577e-4 * H, 4.25588);
	}
	else // if (H <= 20000)
	{
		//216.65:标准同温层温度
		temperature = 216.65;
		pressure = 22632.04 * exp(-1.576885e-4 * (H - 11000));
		density = 0.3639176 * exp(-1.576885e-4 * (H - 11000));
	}
	sound_speed = 20.046796 * sqrt(temperature);
	return std::make_tuple(density, sound_speed);
}