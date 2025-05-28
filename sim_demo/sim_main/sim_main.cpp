#include "getopt.hpp"
#include "sim_engine.h"
#include "fmtlog.h"


int main()
{
	bool help = getarg(false, "-h", "--help", "-?");
	std::string log_filename = getarg("sim_main.log", "-l", "--log-file");
	int log_level = getarg(3, "-L", "--log-level");
	std::string mission = getarg("", "-m", "--mission");

	if (help)
	{
		fmt::print("sim_main [-h|--help|-?] [-l=path|--log-file=path] [-L=number|--log-level=number] [-m=path|--mission=path]\n");
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
	fmtlog::startPollingThread();

    logi("sim_demo start.\n");

    SimEngine sim_engine;

    if (!sim_engine.parse_mission(mission))
    {
		loge("parse mission failed: {}\n", mission);
		return -1;
    }

	sim_engine.start();

	sim_engine.join();

	fmtlog::stopPollingThread();

	return 0;
}
