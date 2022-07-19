#include "MyPlanPathCallback.h"
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Camera>
#include <osg/CameraView>
#include <osg/io_utils>

using namespace osg;
class MyPathCallbackVisitor : public NodeVisitor
{
public:
	MyPathCallbackVisitor(const AnimationPath::ControlPoint& cp, const osg::Vec3d& pivotPoint, bool useInverseMatrix)
		: _cp(cp)
		, _pivotPoint(pivotPoint)
		, _useInverseMatrix(useInverseMatrix)
	{};
	
	virtual void apply(Camera& camera)
	{
		Matrix matrix;
		if (_useInverseMatrix)
			_cp.getInverse(matrix);
		else
			_cp.getMatrix(matrix);

		camera.setViewMatrix(osg::Matrix::translate(-_pivotPoint)*matrix);
	};

	virtual void apply(CameraView& cv)
	{
		if (_useInverseMatrix)
		{
			Matrix matrix;
			_cp.getInverse(matrix);
			cv.setPosition(matrix.getTrans());
			cv.setAttitude(_cp.getRotation().inverse());
			cv.setFocalLength(1.0f / _cp.getScale().x());
		}
		else
		{
			cv.setPosition(_cp.getPosition());
			cv.setAttitude(_cp.getRotation());
			cv.setFocalLength(_cp.getScale().x());
		}
	};

	virtual void apply(MatrixTransform& mt)
	{
		Matrix matrix;
		if (_useInverseMatrix)
			_cp.getInverse(matrix);
		else
			_cp.getMatrix(matrix);
		
		mt.setMatrix(osg::Matrix::translate(_pivotPoint)*matrix);
	};

	virtual void apply(PositionAttitudeTransform& pat)
	{
		if (_useInverseMatrix)
		{
			Matrix matrix;
			_cp.getInverse(matrix);
			pat.setPosition(matrix.getTrans());
			pat.setAttitude(_cp.getRotation().inverse());
			pat.setScale(osg::Vec3d(1.0f / _cp.getScale().x(), 1.0f / _cp.getScale().y(), 1.0f / _cp.getScale().z()));
			pat.setPivotPoint(_pivotPoint);
		}
		else
		{
			pat.setPosition(_cp.getPosition());
			pat.setAttitude(_cp.getRotation());
			pat.setScale(_cp.getScale());
			pat.setPivotPoint(_pivotPoint);
		}
	}

private:
	AnimationPath::ControlPoint _cp;
	osg::Vec3d _pivotPoint;
	bool _useInverseMatrix;
};

MyPlanPathCallback::MyPlanPathCallback(const osg::Vec3d& pivot, const osg::Vec3d& axis, float angularVelocity)
	: _pivotPoint(pivot)
	, _useInverseMatrix(false)
	, _timeOffset(0.0)
	, _timeMultiplier(1.0)
	, _firstTime(DBL_MAX)
	, _latestTime(0.0)
	, _pause(false)
	, _pauseTime(0.0)
{
	_animationPath = new AnimationPath;
	_animationPath->setLoopMode(osg::AnimationPath::LOOP);

	double time0 = 0.0;
	double time1 = osg::PI * 0.5 / angularVelocity;
	double time2 = osg::PI * 1.0 / angularVelocity;
	double time3 = osg::PI * 1.5 / angularVelocity;
	double time4 = osg::PI * 2.0 / angularVelocity;

	osg::Quat rotation0(0.0, axis);
	osg::Quat rotation1(osg::PI * 0.5, axis);
	osg::Quat rotation2(osg::PI * 1.0, axis);
	osg::Quat rotation3(osg::PI * 1.5, axis);

	_animationPath->insert(time0, osg::AnimationPath::ControlPoint(pivot, rotation0));
	_animationPath->insert(time1, osg::AnimationPath::ControlPoint(pivot, rotation1));
	_animationPath->insert(time2, osg::AnimationPath::ControlPoint(pivot, rotation2));
	_animationPath->insert(time3, osg::AnimationPath::ControlPoint(pivot, rotation3));
	_animationPath->insert(time4, osg::AnimationPath::ControlPoint(pivot, rotation0));
}

void MyPlanPathCallback::operator()(Node* node, NodeVisitor* nv)
{
	if (_animationPath.valid() &&
		nv->getVisitorType() == NodeVisitor::UPDATE_VISITOR &&
		nv->getFrameStamp())
	{
		double time = nv->getFrameStamp()->getSimulationTime();
		_latestTime = time;

		if (!_pause)
		{
			// Only update _firstTime the first time, when its value is still DBL_MAX
			if (_firstTime == DBL_MAX) _firstTime = time;
			update(*node);
		}
	}

	// must call any nested node callbacks and continue subgraph traversal.
	NodeCallback::traverse(node, nv);
}

double MyPlanPathCallback::getAnimationTime() const
{
	return ((_latestTime - _firstTime) - _timeOffset) * _timeMultiplier;
}

void MyPlanPathCallback::update(osg::Node& node)
{
	AnimationPath::ControlPoint cp;
	if (_animationPath->getInterpolatedControlPoint(getAnimationTime(), cp))
	{
		MyPathCallbackVisitor apcv(cp, _pivotPoint, _useInverseMatrix);
		node.accept(apcv);
	}
}

void MyPlanPathCallback::reset()
{
#if 1
	_firstTime = DBL_MAX;
	_latestTime = DBL_MAX;
#else
	_firstTime = _latestTime;
	_pauseTime = _latestTime;
#endif
}

void MyPlanPathCallback::setPause(bool pause)
{
	if (_pause == pause)
	{
		return;
	}

	_pause = pause;
	if (_firstTime == DBL_MAX)
	{
		return;
	}

	if (pause)
	{
		_pause = _latestTime;
	}
	else
	{
		_firstTime += (_latestTime - _pauseTime);
	}
}
