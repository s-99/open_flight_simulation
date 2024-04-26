// sim_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "SubSystemAero.h"
#include "dynamic_6dof.h"
#include "engine.h"
#include "fcs.h"
#include "vehicle.h"
#include "aircraft.h"
#include "fmtlog.h"


int main()
{
    fmtlog::setLogFile("sim_demo.log", true);
    fmtlog::setHeaderPattern("+++ {YmdHMSe} {l} {s}\n");
    fmtlog::setLogLevel(fmtlog::INF);

    logi("sim_demo start.\n");

    Aircraft aircraft;
    aircraft._sub_systems.push_back(new Fcs());
    aircraft._sub_systems.push_back(new Engine());
    aircraft._sub_systems.push_back(new SubSystemAero());
    aircraft._sub_systems.push_back(new Dynamic6DOF());

    for (auto* sub_system : aircraft._sub_systems)
    {
        sub_system->_vehicle = &aircraft;
    }

    for (auto* sub_system : aircraft._sub_systems)
    {
        sub_system->init();
    }

    aircraft._data_pool.dump();

    for (auto* sub_system : aircraft._sub_systems)
    {
        sub_system->bind_data();
    }

    double dt = 0.01;
    for (int i = 0; i < 10; i++)
    {
        aircraft.step(dt, i * dt);
        fmtlog::poll();
    }
}
