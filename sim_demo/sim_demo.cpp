// sim_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "aero.h"
#include "dynamic_6dof.h"
#include "engine.h"
#include "fcs.h"
#include "vehicle.h"
#include "aircraft.h"


int main()
{
    Aircraft aircraft;
    aircraft._sub_systems.push_back(new Fcs());
    aircraft._sub_systems.push_back(new Engine());
    aircraft._sub_systems.push_back(new Aero());
    aircraft._sub_systems.push_back(new Dynamic6DOF());

    double dt = 0.01;
    for (int i = 0; i < 10; i++)
    {
        auto* fcs = dynamic_cast<Fcs*>(aircraft._sub_systems[0]);
        auto* aero = dynamic_cast<Aero*>(aircraft._sub_systems[2]);

        aero->_control_surface = fcs->_control_surface;

        aircraft.step(dt, i * dt);
    }
}
