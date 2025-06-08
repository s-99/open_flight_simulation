#pragma once

#include <valarray>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgSim/DOFTransform>
#include <osg/Switch>


class DOFCollector : public osg::NodeVisitor
{
public:
    // 构造函数，设置遍历模式为 TRAVERSE_ALL_CHILDREN
    DOFCollector() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }

    void apply(osg::Node& node) override
    {
        // 检测类名为DOFTransform，保存
        if (strcmp(node.className(), "DOFTransform") == 0)
        {
            _dof_nodes.emplace_back(node.getName(), dynamic_cast<osgSim::DOFTransform*>(&node));
        }
        // 继续递归遍历所有子节点
        traverse(node);
    }

    std::vector<std::pair<std::string, osgSim::DOFTransform*>> _dof_nodes;
};


// Model3d用来管理三维模型
class Model3d
{
public:
	Model3d() = default;

    bool init(const std::string& model_file);
    void update(std::valarray<double>& state);
    std::vector<std::string> get_state_names() const;

    // 从硬盘加载的三维模型
    osg::Node* _model = nullptr;

    // 用于控制模型的显示和隐藏
	osg::Switch* _model_switch = nullptr;

	// 用于控制模型整体运动的 DOFTransform 节点
	osgSim::DOFTransform* _model_transform = nullptr;

	// 舵面 DOFTransform 节点
    std::vector<std::pair<std::string, osgSim::DOFTransform*>> _dof_nodes;

    osg::Vec3d _position = { 0, 0, 0 };

    float _phi = 30;
    float _theta = 0;
    float _psi = 0;
};


class SimEngine;
bool init_3d_models(SimEngine& sim_engine, osg::Group* root, std::vector<Model3d>& models);
