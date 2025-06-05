#include "env3d.h"
#include "getopt.hpp"
#include "sim_engine.h"
#include "fmtlog.h"
#include "model3d.h"
#include "manipulator.h"

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>


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

	// 日志初始化
    fmtlog::setLogFile(log_filename.c_str(), true);
    fmtlog::setHeaderPattern("+++ {YmdHMSe} {l} {s}\n");
    fmtlog::setLogLevel(static_cast<fmtlog::LogLevel>(log_level));
	fmtlog::startPollingThread();

    logi("sim_demo start.\n");

	// 解析任务文件，初始化仿真引擎
    SimEngine sim_engine;

    if (!sim_engine.parse_mission(mission))
    {
		loge("parse mission failed: {}\n", mission);
		return -1;
    }

	// 创建根节点
	osg::Group* root = new osg::Group();

	// 创建所有的飞机三维模型
	std::vector<Model3d> models;
	if (!init_3d_models(sim_engine, root, models))
	{
		loge("init 3D models failed.\n");
		return -1;
	}

	// 创建光源
	init_light(root);

	// 初始化地形模型
	auto* terrain = init_terrain();
	if (!terrain)
	{
		logw("Terrain model not loaded, proceeding without terrain.\n");
		return -1;
	}
	root->addChild(terrain);

	// 创建查看器
	osgViewer::Viewer viewer;
	viewer.setSceneData(root);

	// 设置查看器参数
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// 设置窗口大小
	viewer.setUpViewInWindow(100, 100, 1920, 1080);

	// 启动仿真引擎线程
	sim_engine.start();

	auto& m0 = models[0];

	auto* key_switch = new osgGA::KeySwitchMatrixManipulator;
	auto* node_tracker = new NodeTracker(models[0]._model_transform);
	auto* first_person = new FirstPerson(models[0]._model_transform);
	key_switch->addMatrixManipulator('1', "node tracker", node_tracker);
	key_switch->addMatrixManipulator('2', "first person", first_person);

	viewer.setCameraManipulator(key_switch);

	viewer.addEventHandler(new osgViewer::HelpHandler());

	viewer.realize();

	viewer.getCamera()->setNearFarRatio(0.0001);

	// 启动主循环
	while (!viewer.done() && !sim_engine._finished)
	{
		// 读取飞机状态
		auto vehicle_state = sim_engine.get_vehicle_states();
		for (int i = 0; i < models.size(); i++)
		{
			models[i].update(vehicle_state[i]);
		}

		viewer.frame();
	}

	sim_engine.stop();
	sim_engine.join();

	fmtlog::stopPollingThread();

	return 0;
}
