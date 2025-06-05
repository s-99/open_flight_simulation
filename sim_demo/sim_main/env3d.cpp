#include "env3d.h"

#include "fmtlog.h"

#include <osg/LightSource>
#include <osg/StateAttribute>
#include <osgDB/ReadFile>
#include <osgSim/DOFTransform>


void init_light(osg::Group* root)
{
    // 创建光源（模拟太阳光）
    osg::ref_ptr<osg::Light> light = new osg::Light;
    light->setLightNum(0); // 使用光源 0
    light->setPosition(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f)); // 方向光 (w=0)
    light->setDiffuse(osg::Vec4(1.0f, 1.0f, 0.9f, 1.0f)); // 光源颜色（太阳光）
    light->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f)); // 环境光
    light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); // 镜面反射光

    // 创建光源节点
    osg::ref_ptr<osg::LightSource> light_source = new osg::LightSource;
    light_source->setLight(light.get());

    root->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);
    root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    root->addChild(light_source);
}


osg::Node* init_terrain()
{
    auto* terrain = osgDB::readNodeFile("..\\..\\terrain\\terrain.ive");
    if (!terrain)
    {
		loge("Failed to load terrain model.\n");
        return nullptr;
    }

    auto bound = terrain->getBound();

    float scale = 100000;
    osgSim::DOFTransform* terrain_transform = new osgSim::DOFTransform();
    terrain_transform->addChild(terrain);
    terrain_transform->setCurrentScale({ scale, scale, scale });
    terrain_transform->setCurrentTranslate(-bound.center() * scale);
    terrain_transform->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	return terrain_transform;
}
