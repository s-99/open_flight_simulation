#pragma once

#include <osgSim/DOFTransform>

#include <osgViewer/ViewerEventHandlers>


class FirstPerson : public osgGA::CameraManipulator
{
public:
	FirstPerson(osg::ref_ptr<osgSim::DOFTransform> t)
		: _transform(t)
	{
	}

	void setByMatrix(const osg::Matrixd& matrix) override {}

	void setByInverseMatrix(const osg::Matrixd& matrix) override {}

	// 摄相机坐标系 -> 世界坐标系（摄像机位置）
	osg::Matrixd getMatrix() const override
	{
		return osg::Matrixd::inverse(getInverseMatrix());
	}

	// 世界坐标系 -> 摄相机坐标系 (ViewMatrix)
	osg::Matrixd getInverseMatrix() const override
	{
		osg::Matrix m;
		_transform->computeWorldToLocalMatrix(m, nullptr);
		return m * osg::Matrixd::rotate(osg::inDegrees(-90.0), osg::Vec3d(1, 0, 0));
	}

private:
	osg::ref_ptr<osgSim::DOFTransform> _transform;
};



class NodeTracker : public osgGA::CameraManipulator
{
public:
	NodeTracker(osg::ref_ptr<osgSim::DOFTransform> t)
		: _transform(t)
	{
	}

	void setByMatrix(const osg::Matrixd& matrix) override {}

	void setByInverseMatrix(const osg::Matrixd& matrix) override {}

	// 摄相机坐标系 -> 世界坐标系（摄像机位置）
	osg::Matrixd getMatrix() const override
	{
		return osg::Matrixd::inverse(getInverseMatrix());
	}

	// 世界坐标系 -> 摄相机坐标系 (ViewMatrix)
	osg::Matrixd getInverseMatrix() const override
	{
		// 组合变换矩阵
		return osg::Matrixd::translate(-_transform->getCurrentTranslate())
			* osg::Matrixd::rotate(osg::inDegrees(_yaw), osg::Vec3d(0, 0, 1))
			* osg::Matrixd::rotate(osg::inDegrees(-90.0 - _pitch), osg::Vec3d(1, 0, 0))
			* osg::Matrixd::translate(0, 0, -_dist);
	}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
	{
		switch (ea.getEventType()) {
		case osgGA::GUIEventAdapter::DRAG:
		{
			// 获取鼠标移动量
			float dx = ea.getXnormalized();
			float dy = ea.getYnormalized();

			// 摄像机水平旋转
			_yaw += dx * 0.5;

			// 摄像机俯仰，角度不超过±80度
			_pitch = osg::clampBetween(_pitch + dy * 0.5, -80.0, 80.0);

			return true;
		}
		case osgGA::GUIEventAdapter::SCROLL:
			// 鼠标滚轮缩放
			switch (ea.getScrollingMotion())
			{
			case osgGA::GUIEventAdapter::SCROLL_UP:
				_dist = std::max(1.0, _dist / 1.1);
				break;
			case osgGA::GUIEventAdapter::SCROLL_DOWN:
				_dist = std::min(1000.0, _dist * 1.1);
				break;
			}
			return true;
		case osgGA::GUIEventAdapter::FRAME:
			return false;
		default:
			return false;
		}
	}

private:
	osg::ref_ptr<osgSim::DOFTransform> _transform;
	double _yaw = 0;    // 左右旋转角度
	double _pitch = 0;  // 上下旋转角度
	double _dist = 10; // 摄像机距离
};

