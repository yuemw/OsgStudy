#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgearthqttest.h"

#include <osg/io_utils>
#include <osg/MatrixTransform>
#include <osg/Depth>
#include <osg/AnimationPath>
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osgViewer/Viewer>
#include <osgWidget/Label>
#include <osgWidget/Window>

#include <osgEarth/MapNode>
#include <osgEarth/SpatialReference>
#include <osgEarth/AutoClipPlaneHandler>
#include <osgEarth/LinearLineOfSight>
#include <osgEarth/RadialLineOfSight>
#include <osgEarth/TerrainTileNode>
#include <osgEarth/FileUtils>
#include <osgEarth/GeoData>
#include <osgEarth/EarthManipulator>

QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	void init();
	osg::Camera* backGround(QString sImagePath, int iWidth, int iHeight);
private slots:
	void			on_pushButton_select_clicked();
	void			on_pushButton_clear_clicked();
	void			on_pushButton_mkNode_clicked();
	
	void			initOsgWindow();
	void			initOsgEarthWindow();

private:
	osg::ref_ptr<osg::Node> createSceneGraph();
	void			addGraticule();
	void			addCustomNode();

	osg::Node*			createPlane(osg::Node* node, const osgEarth::GeoPoint& pos,
		const osgEarth::SpatialReference* mapSRS, double radius, double time);
	osg::AnimationPath* createAnimationPath(const osgEarth::GeoPoint& pos, const osgEarth::SpatialReference* mapSRS,
		float radius, double looptime);

	void				startTrackNode(osg::Node* node);

	// 创建一个飞机飞行模型
	void				addPlane();
	osg::AnimationPath* createPlanePath();
	double				getDistance(osg::Vec3d pos1, osg::Vec3d pos2);
	double				getRunTime(osg::Vec3 from, osg::Vec3 to, double speed);
	void				addTail(osg::Vec3 position, osg::MatrixTransform* scale);

	void				addRadarModel();
	std::vector<osg::Vec3d> getRadarLocalPoints();
	osg::ref_ptr<osg::Geode> genCoverSurface(const int row, const int col, const std::vector<osg::Vec3d>& vecLocalPoints, const osg::Vec3f& color);
	osg::ref_ptr<osg::Geode> genCoverLine(const int row, const int col, const std::vector<osg::Vec3d>& vecLocalPoints, const osg::Vec3f& color);


	void setViewPointPosition(double lon, double lat, double alt,
		double heading = 0, double pitch = -90.0, double distance_Rnge = 7500000);
private:
    Ui::OsgEarthQtTest				ui;
	osgQOpenGLWidget*				_pOsgWidget;
	osg::ref_ptr<osgViewer::Viewer> _viewer;

	QString							_fileName;

	osg::ref_ptr<osg::Group>		m_rootNode;
	osg::ref_ptr<osgEarth::MapNode> m_mapNode;
	osg::ref_ptr<osgEarth::EarthManipulator> m_em;
};
