// sim_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "fmtlog.h"
#include "sim_engine.h"
#include "getopt.hpp"

int main()
{
	bool help = getarg(false, "-h", "--help", "-?");
	std::string log_filename = getarg("sim_demo.log", "-l", "--log-file");
	int log_level = getarg(3, "-L", "--log-level");
	std::string mission = getarg("", "-m", "--mission");

	if (help)
	{
		fmt::print("sim_demo [-h|--help|-?] [-l=path|--log-file=path] [-L=number|--log-level=number] [-m=path|--mission=path]\n");
		return 0;
	}
	if (mission.empty())
	{
		fmt::print("mission file not specified\n");
		return -1;
	}

    fmtlog::setLogFile(log_filename.c_str(), true);
    fmtlog::setHeaderPattern("+++ {YmdHMSe} {l} {s}\n");
    fmtlog::setLogLevel(static_cast<fmtlog::LogLevel>(log_level));

    logi("sim_demo start.\n");

    SimEngine sim_engine;

    if (!sim_engine.parse_mission(mission))
    {
		loge("parse mission failed: {}\n", mission);
		return -1;
    }

	int i = 0;
	while (sim_engine.step())
	{
		i++;
		if (i % 100 == 0)
		{
			fmt::print("sim time: {:.2f}\n", sim_engine._time);
		}
		fmtlog::poll();
	}
}
