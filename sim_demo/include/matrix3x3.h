#pragma once

// 本程序在copilot辅助下生成

#include "vec3.h"

// 3x3矩阵
// 行优先存储
class Matrix3x3
{
protected:
	double _data[9] = {};

public:
	Matrix3x3() = default;
	Matrix3x3(std::initializer_list<double> list)
	{
		int i = 0;
		for (const double it : list)
		{
			_data[i] = it;
			++i;
		}
	}

	Matrix3x3(const Matrix3x3& other)
	{
		for (int i = 0; i < 9; ++i)
		{
			_data[i] = other._data[i];
		}
	}

	~Matrix3x3() = default;

	double operator ()(const int row, const int col) const
	{
		return _data[row * 3 + col];
	}

	double& operator ()(const int row, const int col)
	{
		return _data[row * 3 + col];
	}

	// 矩阵乘标量
	Matrix3x3 operator *(const double other) const
	{
		return {
			_data[0] * other, _data[1] * other, _data[2] * other,
			_data[3] * other, _data[4] * other, _data[5] * other,
			_data[6] * other, _data[7] * other, _data[8] * other
		};
	}

	// 矩阵除标量
	Matrix3x3 operator /(const double other) const
	{
		return {
			_data[0] * other, _data[1] * other, _data[2] * other,
			_data[3] * other, _data[4] * other, _data[5] * other,
			_data[6] * other, _data[7] * other, _data[8] * other
		};
	}

	// 矩阵乘矢量
	Vec3 operator *(const Vec3& other) const
	{
		return {
			_data[0] * other[0] + _data[1] * other[1] + _data[2] * other[2],
			_data[3] * other[0] + _data[4] * other[1] + _data[5] * other[2],
			_data[6] * other[0] + _data[7] * other[1] + _data[8] * other[2]
		};
	}

	// 矩阵乘法
	Matrix3x3 operator *(const Matrix3x3& other) const
	{
		return {
			_data[0] * other._data[0] + _data[1] * other._data[3] + _data[2] * other._data[6],
			_data[0] * other._data[1] + _data[1] * other._data[4] + _data[2] * other._data[7],
			_data[0] * other._data[2] + _data[1] * other._data[5] + _data[2] * other._data[8],
			_data[3] * other._data[0] + _data[4] * other._data[3] + _data[5] * other._data[6],
			_data[3] * other._data[1] + _data[4] * other._data[4] + _data[5] * other._data[7],
			_data[3] * other._data[2] + _data[4] * other._data[5] + _data[5] * other._data[8],
			_data[6] * other._data[0] + _data[7] * other._data[3] + _data[8] * other._data[6],
			_data[6] * other._data[1] + _data[7] * other._data[4] + _data[8] * other._data[7],
			_data[6] * other._data[2] + _data[7] * other._data[5] + _data[8] * other._data[8]
		};
	}

	// 就地转置
	void transpose()
	{
		std::swap(_data[1], _data[3]);
		std::swap(_data[2], _data[6]);
		std::swap(_data[5], _data[7]);
	}

	// 转置矩阵
	Matrix3x3 transposed() const
	{
		return {
			_data[0], _data[3], _data[6],
			_data[1], _data[4], _data[7],
			_data[2], _data[5], _data[8]
		};
	}

	// 伴随矩阵
	Matrix3x3 adj() const
	{
		return {
			_data[4] * _data[8] - _data[5] * _data[7],
			_data[2] * _data[7] - _data[1] * _data[8],
			_data[1] * _data[5] - _data[2] * _data[4],
			_data[5] * _data[6] - _data[3] * _data[8],
			_data[0] * _data[8] - _data[2] * _data[6],
			_data[2] * _data[3] - _data[0] * _data[5],
			_data[3] * _data[7] - _data[4] * _data[6],
			_data[1] * _data[6] - _data[0] * _data[7],
			_data[0] * _data[4] - _data[1] * _data[3]
		};
	}

	// 行列式
	double det() const
	{
		return _data[0] * _data[4] * _data[8] + _data[1] * _data[5] * _data[6] + _data[2] * _data[3] * _data[7]
			- _data[2] * _data[4] * _data[6] - _data[1] * _data[3] * _data[8] - _data[0] * _data[5] * _data[7];
	}

	// 逆矩阵
	Matrix3x3 inverse() const
	{
		Matrix3x3 adj = this->adj();
		adj.transpose();
		const double det = this->det();
		return adj / det;
	}

	// 转换为字符串
	std::string to_string() const
	{
		return std::format("[{},{},{},\n {},{},{}\n {},{},{}]",
			_data[0], _data[1], _data[2],
			_data[3], _data[4], _data[5],
			_data[6], _data[7], _data[8]);
	}
};
