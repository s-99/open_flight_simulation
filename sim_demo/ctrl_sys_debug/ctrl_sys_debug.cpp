
#include <iostream>
#include <fstream>
#include "fmtlog.h"
#include "block_simple.h"
#include "getopt.hpp"
#include "ode_solver_runge_kutta.h"


int main()
{
	bool help = getarg(false, "-h", "--help", "-?");
	std::string log_filename = getarg("ctrl_sys_debug.log", "-l", "--log-file");
	int log_level = getarg(3, "-L", "--log-level");
	std::string config = getarg("", "-c", "--config");
	std::string solver = getarg("runge-kutta:1", "-s", "--solver");
	std::string output_file = getarg("ctrl_sys_debug.csv", "-o", "--output");

	if (help)
	{
		fmt::print("ctrl_sys_debug [-h|--help|-?] [-l=path|--log-file=path] [-L=number|--log-level=number]\n"
			"[-c=path|--config=path] [-s=solver|--solver=solver] [-o=path|--output=path]\n");
		return 0;
	}
	if (config.empty())
	{
		fmt::print("config file not specified\n");
		return -1;
	}

    fmtlog::setLogFile(log_filename.c_str(), true);
    fmtlog::setHeaderPattern("+++ {YmdHMSe} {l} {s}\n");
    fmtlog::setLogLevel(static_cast<fmtlog::LogLevel>(log_level));

    logi("ctrl_sys_debug start.\n");

	BlockSubSystem sys;

	// 解析控制律配置文件
	std::ifstream f(config);
	try
	{
		auto data = json::parse(f);
		if (!sys.init(data))
		{
			fmt::print("init system failed\n");
			return -1;	
		}
	}
	catch (json::parse_error& e)
	{
		fmt::print("parse json error: {}\n", e.what());
		return -1;
	}

	// 设置求解器
	if (solver.substr(0, 12) == "runge-kutta:")
	{
		int order = strtol(solver.substr(12).c_str(), nullptr, 10);
		sys.set_solver(new OdeSolverRungeKuttaN(order));
	}
	else
	{
		fmt::print("unknown solver: {}\n", solver);
		return -1;
	}

	FILE* fout = fopen(output_file.c_str(), "wt");

	fprintf(fout, "t");
	for (auto& i: sys._input_sig)
	{
		fprintf(fout, ",%s", i._name.c_str());
	}
	for (auto& o : sys._output_sig)
	{
		fprintf(fout, ",%s", o._name.c_str());
	}
	fprintf(fout, "\n");

	double t = 0;
	double dt = 0.01;
	int i = 0;

	while (t < 10)
	{
		sys._input_sig[0]._value = sin(t);
		sys._input_sig[1]._value = t > 5 ? 0.5 : 0;

		sys.step(dt, t);
		fprintf(fout, "%.3f", t);
		for (auto& i : sys._input_sig)
		{
			fprintf(fout, ",%g", i._value);
		}
		for (auto& o : sys._output)
		{
			fprintf(fout, ",%g", o);
		}
		fprintf(fout, "\n");

		if (i % 100 == 0)
		{
			fmt::print("sim time: {:.2f}\n", t);
		}
		t += dt;
		i++;
		fmtlog::poll();
	}
	fclose(fout);
}
