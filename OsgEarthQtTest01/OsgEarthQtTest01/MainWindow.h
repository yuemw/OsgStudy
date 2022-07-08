#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgearthqttest.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgEarth/MapNode>

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

	osg::Camera* backGround(QString sImagePath, int iWidth, int iHeight);
private slots:
	void			on_pushButton_select_clicked();
	void			on_pushButton_clear_clicked();
	void			on_pushButton_mkNode_clicked();
	
	void			initOsgWindow();

private:
	osg::ref_ptr<osg::Node> createSceneGraph();
	void			initOsgEarthWindow();

private:
    Ui::OsgEarthQtTest			ui;
	osgQOpenGLWidget*			_pOsgWidget;
	osg::ref_ptr<osgViewer::Viewer> _viewer;

	QString						_fileName;

	osg::ref_ptr<osg::Group>	m_rootNode;
	osg::ref_ptr<osgEarth::MapNode> m_MapNode;
	osg::ref_ptr<osgEarth::Map>		m_Map;
};
