#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgearthqttest.h"

#include <osg/io_utils>
#include <osg/MatrixTransform>
#include <osg/Depth>
#include <osg/AnimationPath>
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osgViewer/Viewer>
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

	void				addPlane();
	void				startTrackNode(osg::Node* node);

private:
    Ui::OsgEarthQtTest				ui;
	osgQOpenGLWidget*				_pOsgWidget;
	osg::ref_ptr<osgViewer::Viewer> _viewer;

	QString							_fileName;

	osg::ref_ptr<osg::Group>		m_rootNode;
	osg::ref_ptr<osgEarth::MapNode> m_mapNode;
	osg::ref_ptr<osgEarth::EarthManipulator> m_em;
};
