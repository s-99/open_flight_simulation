#include "model3d.h"

#include "fmtlog.h"

#include "sim_engine.h"


bool Model3d::init(const std::string& model_file)
{
	_model = osgDB::readNodeFile(model_file);
	if (!_model)
	{
		loge("Model3d::init: failed to load model file: {}\n", model_file);
		return false;
	}

	// 查找舵面 DOFTransform 节点
	DOFCollector collector;
	_model->accept(collector);
	_dof_nodes = collector._dof_nodes;

	// 飞机的 DOFTransform 节点
	_model_transform = new osgSim::DOFTransform();
	_model_transform->setName("ModelTransform");
	_model_transform->addChild(_model);

	_model_switch = new osg::Switch();
	_model_switch->setName("ModelSwitch");
	_model_switch->addChild(_model_transform);

	return true;
}


void Model3d::update(std::valarray<double>& state)
{
	// state[0/1/2] = x/y/z
	// state[3/4/5] = phi/theta/psi (roll/pitch/yaw)
	_position = { state[1], state[0], -state[2] };
	_phi = static_cast<float>(state[3]);
	_theta = static_cast<float>(state[4]);
	_psi = static_cast<float>(state[5]);

	_model_transform->setCurrentTranslate(_position);
	_model_transform->setCurrentHPR({ -osg::inDegrees(_psi), osg::inDegrees(_theta), osg::inDegrees(_phi) });
	_model_transform->setHPRMultOrder(osgSim::DOFTransform::HPR);
	_model_transform->setCurrentScale({ 1, 1, 1});

	// state[6...] = control surface
	int i = 6;
	for (const auto& [name, dof] : _dof_nodes)
	{
		dof->setCurrentHPR({ 0, osg::inDegrees(static_cast<float>(state[i])), 0});
		i++;
	}
}


std::vector<std::string> Model3d::get_state_names() const
{
	std::vector<std::string> state_names = {
		"x", "y", "z", // 位置
		"phi", "theta", "psi" // 姿态角
	};
	for (const auto& [name, dof] : _dof_nodes)
	{
		state_names.push_back(name); // 舵面名称
	}
	return state_names;
}


// 加载所有的3D模型，并添加到root场景树上，三维模型对象保存到models中
bool init_3d_models(SimEngine& sim_engine, osg::Group* root, std::vector<Model3d>& models)
{
	models.resize(sim_engine._vehicles.size());
	for (size_t i = 0; i < sim_engine._vehicles.size(); i++)
	{
		auto* vehicle = sim_engine._vehicles[i];
		if (vehicle->_3d_model.empty())
		{
			logw("No 3D model specified for vehicle[{}]\n", i);
			return false;
		}
		if (!models[i].init(vehicle->_3d_model))
		{
			loge("Failed to load 3D model for vehicle[{}]: {}\n", i, vehicle->_3d_model);
			return false;
		}
		logi("Loaded 3D model for vehicle[{}]: {}\n", i, vehicle->_3d_model);
		// 添加到场景树
		root->addChild(models[i]._model_switch);
	}

	// 搜集模型需要的仿真数据名称
	std::vector<std::vector<std::string>> vehicle_state_names;
	for (const auto& model : models)
	{
		vehicle_state_names.emplace_back(model.get_state_names());
	}

	// 通知仿真引擎绑定数据
	if (!sim_engine.bind_vehicle_state(vehicle_state_names))
	{
		loge("bind vehicle state failed.\n");
		return false;
	}

	return true;
}