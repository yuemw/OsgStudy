#pragma once
#include<osgQt/GraphicsWindowQt>

#include <QtCore/QTimer>
#include <QResizeEvent>

#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>

class OsgViewrWidget :
	public osgQt::GLWidget, public osgViewer::Viewer
{
	Q_OBJECT

public:
	OsgViewrWidget(QWidget *parent);
	~OsgViewrWidget(void);

	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void openFile();


protected:
	//QTimer _timer;
};

