#pragma once

#include "vec3.h"

#define _USE_MATH_DEFINES
#include <math.h>


class Quaternion
{
protected:
	// s, x, y, z
	double _data[4] = { 0.0, 0.0, 0.0, 0.0};

public:
	Quaternion() = default;

	Quaternion(const double s, const double x, const double y, const double z)
	{
		_data[0] = s;
		_data[1] = x;
		_data[2] = y;
		_data[3] = z;
	}

	Quaternion(const Quaternion& other)
	{
		_data[0] = other._data[0];
		_data[1] = other._data[1];
		_data[2] = other._data[2];
		_data[3] = other._data[3];
	}

	Quaternion(const Vec3& other)
	{
		_data[0] = 0.0;
		_data[1] = other[0];
		_data[2] = other[1];
		_data[3] = other[2];
	}

	~Quaternion() = default;

	Quaternion& operator =(const Quaternion& other)
	{
		if (this == &other)
			return *this;
		_data[0] = other._data[0];
		_data[1] = other._data[1];
		_data[2] = other._data[2];
		_data[3] = other._data[3];
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

	// ��Ԫ���˷�
	Quaternion operator *(const Quaternion& other) const
	{
		return {
			_data[0] * other._data[0] - _data[1] * other._data[1] - _data[2] * other._data[2] - _data[3] * other._data[3],
			_data[0] * other._data[1] + _data[1] * other._data[0] + _data[2] * other._data[3] - _data[3] * other._data[2],
			_data[0] * other._data[2] - _data[1] * other._data[3] + _data[2] * other._data[0] + _data[3] * other._data[1],
			_data[0] * other._data[3] + _data[1] * other._data[2] - _data[2] * other._data[1] + _data[3] * other._data[0]
		};
	}

	Quaternion operator +(const Quaternion& other) const
	{
		return {
			_data[0] + other._data[0],
			_data[1] + other._data[1],
			_data[2] + other._data[2],
			_data[3] + other._data[3]
		};
	}

	void operator +=(const Quaternion& other)
	{
		_data[0] += other._data[0];
		_data[1] += other._data[1];
		_data[2] += other._data[2];
		_data[3] += other._data[3];
	}

	Quaternion operator -(const Quaternion& other) const
	{
		return {
			_data[0] + other._data[0],
			_data[1] + other._data[1],
			_data[2] + other._data[2],
			_data[3] + other._data[3]
		};
	}

	void operator -=(const Quaternion& other)
	{
		_data[0] += other._data[0];
		_data[1] += other._data[1];
		_data[2] += other._data[2];
		_data[3] += other._data[3];
	}

	Quaternion operator -() const
	{
		return {
			-_data[0],
			-_data[1],
			-_data[2],
			-_data[3]
		};
	}

	// ��Ԫ���˱���
	Quaternion operator *(const double& other) const
	{
		return {
			_data[0] * other,
			_data[1] * other,
			_data[2] * other,
			_data[3] * other
		};
	}

	// ��Ԫ�����
	double dot(const Quaternion& other) const
	{
		return _data[0] * other._data[0] + _data[1] * other._data[1] + _data[2] * other._data[2] + _data[3] * other._data[3];
	}

	// ������Ԫ��
	Quaternion conjugate() const
	{
		return { _data[0], -_data[1], -_data[2], -_data[3] };
	}

	// ����Ԫ��
	Quaternion inverse() const
	{
		return conjugate() * (1.0 / length2());
	}

	// ʵ��
	double real() const
	{
		return _data[0];
	}

	// �鲿��Ϊһ��ʸ������
	Vec3 vector() const
	{
		return { _data[1], _data[2], _data[3] };
	}

	// ʹ����Ԫ����ʸ��������ת���������ϵ��ת������Ҫ����Ԫ��ȡ����
	Vec3 operator ^(const Vec3& other) const
	{
		return (*this * Quaternion(other) * conjugate()).vector();
	}

	// ��Ԫ������
	double length() const
	{
		return std::sqrt(_data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2] + _data[3] * _data[3]);
	}

	// ��Ԫ�����ȵ�ƽ��
	double length2() const
	{
		return _data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2] + _data[3] * _data[3];
	}

	// ��Ԫ����һ��
	double normalize()
	{
		const double len = length();
		if (len > 0.0)
		{
			_data[0] /= len;
			_data[1] /= len;
			_data[2] /= len;
			_data[3] /= len;
		}
		return len;
	}

	std::string to_string() const
	{
		return std::format("({:8.5f},{:8.5f},{:8.5f},{:8.5f})", _data[0], _data[1], _data[2], _data[3]);
	}

	// ŷ����ת��Ԫ��
	void from_euler(const Vec3& euler)
	{
		const double phi = euler[0];
		const double theta = euler[1];
		const double psi = euler[2];
		const double cy = std::cos(psi * 0.5);
		const double sy = std::sin(psi * 0.5);
		const double cp = std::cos(theta * 0.5);
		const double sp = std::sin(theta * 0.5);
		const double cr = std::cos(phi * 0.5);
		const double sr = std::sin(phi * 0.5);

		_data[0] = cy * cp * cr + sy * sp * sr;
		_data[1] = cy * cp * sr - sy * sp * cr;
		_data[2] = sy * cp * sr + cy * sp * cr;
		_data[3] = sy * cp * cr - cy * sp * sr;
	}

	void from_euler_degree(const Vec3& euler)
	{
		from_euler(euler * (M_PI / 180.0));
	}

	// ��Ԫ��תŷ����
	// ����false��ʾ����㣬��ʱ��ֻ����theta��������phi��psi
	bool to_euler(Vec3& euler) const
	{
		const double k = 2 * (_data[0] * _data[2] - _data[1] * _data[3]);
		if (abs(k) < 0.9999999)
		{
			euler[0] = std::atan2(2 * (_data[0] * _data[1] + _data[2] * _data[3]),
				(_data[0] * _data[0] - _data[1] * _data[1] - _data[2] * _data[2] + _data[3] * _data[3]));
			euler[1] = std::asin(k);
			euler[2] = std::atan2(2 * (_data[1] * _data[2] + _data[0] * _data[3]),
				(_data[0] * _data[0] + _data[1] * _data[1] - _data[2] * _data[2] - _data[3] * _data[3]));
			return true;
		}
		else
		{
			euler[1] = k > 0 ? M_PI_2 : -M_PI_2;
		}
		return false;
	}

	bool to_euler_degree(Vec3& euler_degree) const
	{
		auto euler = euler_degree * (M_PI / 180.0);
		const auto ret = to_euler(euler);
		euler_degree = euler * (180.0 / M_PI);
		return ret;
	}

	Quaternion derivative(const Vec3& omega) const
	{
		return (*this * Quaternion(omega)) * 0.5;
	}
};
