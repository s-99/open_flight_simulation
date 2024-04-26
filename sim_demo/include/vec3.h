#pragma once

// 本程序在copilot辅助下生成

#include <cmath>
#include <string>
#include <format>

// 三维向量
class Vec3
{
protected:
	double _data[3] = { 0.0, 0.0, 0.0 };

public:
	Vec3() = default;

	Vec3(const double x, const double y, const double z)
	{
		_data[0] = x;
		_data[1] = y;
		_data[2] = z;
	}

	Vec3(const Vec3& other)
	{
		_data[0] = other._data[0];
		_data[1] = other._data[1];
		_data[2] = other._data[2];
	}

	~Vec3() = default;

	Vec3& operator =(const Vec3& other)
	{
		if (this == &other)
			return *this;
		_data[0] = other._data[0];
		_data[1] = other._data[1];
		_data[2] = other._data[2];
		return *this;
	}

	double operator [](const int index) const
	{
		return _data[index];
	}

	double& operator [](const int index)
	{
		return _data[index];
	}

	Vec3 operator +(const double other) const
	{
		return { _data[0] + other, _data[1] + other, _data[2] + other };
	}

	Vec3 operator -(const double other) const
	{
		return { _data[0] - other, _data[1] - other, _data[2] - other };
	}

	Vec3 operator *(const double scalar) const
	{
		return {_data[0] * scalar, _data[1] * scalar, _data[2] * scalar};
	}

	Vec3 operator /(const double scalar) const
	{
		return { _data[0] / scalar, _data[1] / scalar, _data[2] / scalar };
	}

	double operator *(const Vec3& other) const
	{
		return _data[0] * other._data[0] + _data[1] * other._data[1] + _data[2] * other._data[2];
	}

	Vec3 operator ^(const Vec3& other) const
	{
		return {
			_data[1] * other._data[2] - _data[2] * other._data[1],
			_data[2] * other._data[0] - _data[0] * other._data[2],
			_data[0] * other._data[1] - _data[1] * other._data[0]
		};
	}

	Vec3 operator +(const Vec3& other) const
	{
		return {_data[0] + other._data[0], _data[1] + other._data[1], _data[2] + other._data[2]};
	}

	Vec3& operator +=(const Vec3& other)
	{
		_data[0] += other._data[0];
		_data[1] += other._data[1];
		_data[2] += other._data[2];
		return *this;
	}

	Vec3 operator -(const Vec3& other) const
	{
		return {_data[0] - other._data[0], _data[1] - other._data[1], _data[2] - other._data[2]};
	}

	Vec3& operator -=(const Vec3& other)
	{
		_data[0] -= other._data[0];
		_data[1] -= other._data[1];
		_data[2] -= other._data[2];
		return *this;
	}

	double length() const
	{
		return std::sqrt(_data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2]);
	}

	double length2() const
	{
		return _data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2];
	}

	std::string to_string() const
	{
		return std::format("({:8.5f},{:8.5f},{:8.5f})", _data[0], _data[1], _data[2]);
	}
};

