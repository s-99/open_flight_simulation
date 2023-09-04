#include <iostream>
#include "vec3.h"
#include "matrix3x3.h"
#include "quat.h"


int main()
{
	Quaternion q{ 1, 2, 3, 4 };
	std::cout << "q*q=" << (q * q).to_string() << std::endl;
	q.normalize();
	std::cout << "q.normalize=" << q.to_string() << std::endl;
	Vec3 v{ 4, 2, 4 };
	std::cout << "q*v=" << (q * v).to_string() << std::endl;
	std::cout << "q^v=" << (q ^ v).to_string() << std::endl;

	Vec3 ea{ 10,20,30 };
	Vec3 eb;
	q.from_euler_degree(ea);
	std::cout << "ea=" << ea.to_string() << std::endl;
	std::cout << "q=" << q.to_string() << std::endl;
	q.to_euler_degree(eb);
	std::cout << "eb=" << eb.to_string() << std::endl;

	ea[1] = -90;
	q.from_euler_degree(ea);
	q.to_euler_degree(eb);
	std::cout << "eb=" << eb.to_string() << std::endl;

	Quaternion{ 0.501317, 0.471347, 0.527098, 0.498680 }.to_euler_degree(eb);
	std::cout << "e=" << eb.to_string() << std::endl;

	Vec3 euler{ 0.0, 0.0, 0.0 };


	Quaternion q1;
	q1.from_euler_degree(euler);
	std::cout << "euler[0]=" << euler.to_string() << std::endl;

	// p/q/r
	Vec3 omega{0.0, 1.0 * M_PI / 180, 0.0};
	for (int i = 1; i < 10; i++)
	{
		q1 += q1.derivative(omega);
		q1.to_euler_degree(euler);
		std::cout << "euler[" << i << "]=" << euler.to_string()  << "        q=" << q1.to_string() << std::endl;
	}
}
