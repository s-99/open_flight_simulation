// sim_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "fmtlog.h"
#include "sim_engine.h"


int main()
{
    fmtlog::setLogFile("sim_demo.log", true);
    fmtlog::setHeaderPattern("+++ {YmdHMSe} {l} {s}\n");
    fmtlog::setLogLevel(fmtlog::ERR);

    logi("sim_demo start.\n");

    SimEngine sim_engine;

    if (!sim_engine.parse_mission("demo_mission.json"))
    {
		loge("parse mission failed\n");
		return -1;
    }

	while (sim_engine.step())
	{
		fmtlog::poll();
	}
}
