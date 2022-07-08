#include "OsgViewrWidget.h"
#include <QFileDialog>



OsgViewrWidget::OsgViewrWidget(QWidget *parent) :
	osgQt::GLWidget(parent)
{
	this->_gw = new osgQt::GraphicsWindowQt(this);

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(_gw);

	camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	camera->setClearDepth(1.0);
	setCamera(camera);

	addEventHandler(new osgViewer::StatsHandler);

	osgGA::TrackballManipulator* trackBallMpl = new osgGA::TrackballManipulator();
	setCameraManipulator(trackBallMpl);
	openFile();
	//connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
	//_timer.start( 10 );
}


OsgViewrWidget::~OsgViewrWidget(void)
{
}

void OsgViewrWidget::paintEvent(QPaintEvent* event)
{
	frame();

	// 连续刷新，不用timer
	update();
}

void OsgViewrWidget::resizeEvent(QResizeEvent* event)
{
	GLWidget::resizeEvent(event);

	if (_gw->getTraits()->height > 0)
	{
		getCamera()->setViewport(0, 0, _gw->getTraits()->width, _gw->getTraits()->height);
		getCamera()->setProjectionMatrixAsPerspective(
			30.0f, static_cast<double>(_gw->getTraits()->width) / static_cast<double>(_gw->getTraits()->height), 1.0f, 10000.0f);

	}

	frame();
}

void OsgViewrWidget::openFile()
{
	QString file_name = QFileDialog::getOpenFileName(this,
		tr("Open File"),
		"",
		"",
		0);
	if (!file_name.isNull())
	{
		//fileName是文件名
		/*osg::ref_ptr<osg::Node> scenenode = osgDB::readNodeFile("cow.osg");*/
		std::string path = std::string((const char*)file_name.toLocal8Bit());

		osg::ref_ptr<osg::Node> scenenode = osgDB::readNodeFile(path);
		/*osg::ref_ptr<osg::Node> scenenode = osgDB::readNodeFile(file_name.toStdString());*/

		setSceneData(scenenode);
		getCamera()->dirtyBound();
	}

}


