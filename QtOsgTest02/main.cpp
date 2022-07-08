#include "mainwindow.h"
#include <QApplication>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <QFileInfo>
#include <QSettings>
#include <osgUtil/Optimizer>   
#include <osgEarth/Common>

void appSetup(const QString organizationName)
{
	// set up application name
	QFileInfo applicationFile(QApplication::applicationFilePath());

	// These allow us to simply construct a "QSettings" object without arguments
	qApp->setOrganizationDomain("mil.army.arl");
	qApp->setApplicationName(applicationFile.baseName());
	qApp->setOrganizationName(organizationName);
	qApp->setApplicationVersion(__DATE__ __TIME__);

	// Look up the last directory where the user opened files.
	// set it if it hasn't been set.
	QSettings settings;
	if (!settings.allKeys().contains("currentDirectory"))
		settings.setValue("currentDirectory",
			applicationFile.path());
}

int main(int argc, char *argv[])
{
	osgEarth::initialize();

	QApplication a(argc, argv);
	appSetup("Army Research Laboratory");
// 	MainWindow w;
// 	w.show();

// ¼ÓÔØearthÎÄ¼þÂß¼­
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("simple.earth");

	if (node)
	{
		root->addChild(node.get());
		osgUtil::Optimizer optimizer;
		optimizer.optimize(root.get());
		viewer->setSceneData(root.get());
		viewer->realize();
		viewer->run();

		return 0;
	}

	return a.exec();
}
