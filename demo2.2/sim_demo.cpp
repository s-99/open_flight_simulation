#include <iostream>
#include "vec3.h"
#include "matrix3x3.h"


int main()
{
	Vec3 a(1, 2, 3);
	Vec3 b(4, 5, 6);
	Vec3 c(a);
	Vec3 d = b;
	double x = a[0];
	a[0] = 3.5;
	auto e = a + b;

	std::cout << "a = " << a.to_string() << std::endl;
	std::cout << "b = " << b.to_string() << std::endl;
	std::cout << "c = " << c.to_string() << std::endl;
	std::cout << "d = " << d.to_string() << std::endl;
	std::cout << "e = " << e.to_string() << std::endl;
	std::cout << "a * b = " << a * b << std::endl;
	std::cout << "a ^ b = " << (a ^ b).to_string() << std::endl;

	Matrix3x3 m{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	std::cout << "m = \n" << m.to_string() << std::endl;
	std::cout << "m * a = " << (m * a).to_string() << std::endl;
	std::cout << "m * m = " << (m * m).to_string() << std::endl;
	std::cout << "m.inverse() = " << m.inverse().to_string() << std::endl;
}
