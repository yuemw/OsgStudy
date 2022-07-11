#include "MainWindow.h"
#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>
 
#include <osg/Node>   
#include <osg/Geode>   
#include <osg/Group>   
#include <osgDB/WriteFile>  

#include <osgUtil/Optimizer>   
#include <osgEarth/Utils>
#include <osgEarth/EarthManipulator>
#include <osgEarth/Sky>
#include <osgEarth/DateTime>

#include <osgEarth/MapNode>
#include <osgEarth/ExampleResources>
#include <osgEarth/ImageOverlay>
#include <osgEarth/CircleNode>
#include <osgEarth/RectangleNode>
#include <osgEarth/EllipseNode>
#include <osgEarth/PlaceNode>
#include <osgEarth/LabelNode>
#include <osgEarth/LocalGeometryNode>
#include <osgEarth/FeatureNode>
#include <osgEarth/ImageOverlayEditor>
#include <osgEarth/GeometryFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	ui.lineEdit_file->setEnabled(false);

	_pOsgWidget = new osgQOpenGLWidget(this);
	
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(_pOsgWidget);
	ui.widget_2->setLayout(layout);

	connect(_pOsgWidget, SIGNAL(initialize()), this, SLOT(initOsgWindow()));
}

void MainWindow::init()
{
}

osg::Camera* MainWindow::backGround(QString sImagePath, int iWidth, int iHeight)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	camera->setAllowEventFocus(false);
	camera->setProjectionMatrixAsOrtho2D(0, iWidth, 0, iHeight);
	camera->setViewport(0, 0, iWidth, iHeight);

	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setViewMatrix(osg::Matrix::identity());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Geometry> geomotry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
	vertex->push_back(osg::Vec3(0, 0, 0));
	vertex->push_back(osg::Vec3(iWidth, 0, 0));
	vertex->push_back(osg::Vec3(iWidth, iHeight, 0));
	vertex->push_back(osg::Vec3(0, iHeight, 0));
	geomotry->setVertexArray(vertex);

	osg::ref_ptr<osg::Vec2Array> texCoord = new osg::Vec2Array;
	texCoord->push_back(osg::Vec2(0.0, 0.0));
	texCoord->push_back(osg::Vec2(1.0, 0.0));
	texCoord->push_back(osg::Vec2(1.0, 1.0));
	texCoord->push_back(osg::Vec2(0.0, 1.0));
	geomotry->setTexCoordArray(0, texCoord);

	geomotry->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4));

	osg::ref_ptr<osg::Image> img = osgDB::readImageFile(sImagePath.toStdString());

	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
	tex->setImage(img);

	geomotry->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex.get(), osg::StateAttribute::ON);

	geode->addDrawable(geomotry);
	camera->addChild(geode);

	return camera.release();
}

void MainWindow::on_pushButton_select_clicked()
{
	_fileName = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"),
		"F:/program/osg/OsgCore/data", "Osg(*.osg)");
	ui.lineEdit_file->setText(_fileName);
	initOsgWindow();
}

void MainWindow::on_pushButton_clear_clicked()
{
	_fileName.clear();
	ui.lineEdit_file->setText("");
	initOsgWindow();
}

void MainWindow::on_pushButton_mkNode_clicked()
{
	initOsgEarthWindow();

	addCustomNode();
	return;

// 	osg::ref_ptr<osg::Node> root = createSceneGraph();
// 	if (!root.valid())
// 		return;
// 
// 	_viewer = _pOsgWidget->getOsgViewer();
// 	_viewer->setSceneData(root);
}

osg::ref_ptr<osg::Node> MainWindow::createSceneGraph()
{
	// 创建一个用于保存几何信息的对象
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	// 创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, 1.f));
	v->push_back(osg::Vec3(-1.f, 0.f, 1.f));

	// 创建四种颜色的数组
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
	c->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

	// 为唯一的法线创建一个数组
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());

	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, -1.f, 0.f));

	// 由保存的数据绘制四个顶点的多边形
	geom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	// 向 Geode 类添加几何体（Drawable）并返回 Geode 
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());
	return geode.get();
}

void MainWindow::initOsgWindow()
{
	_viewer = _pOsgWidget->getOsgViewer();
	_viewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
	_viewer->setCameraManipulator(new osgGA::TrackballManipulator);
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(backGround("F:/program/osg/testcode/OsgEarthQtTest01/OsgEarthQtTest01/sky.jpg", _pOsgWidget->width(), _pOsgWidget->height()));
	group->addChild(osgDB::readNodeFile(_fileName.toStdString()));

	_viewer->setSceneData(group);
}

void MainWindow::initOsgEarthWindow()
{
	osgEarth::initialize();

	m_rootNode = new osg::Group();
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("mymap.earth");
	if (nullptr == node)
		return;

	if (node.valid())
	{
		m_MapNode = osgEarth::MapNode::get(node.get());
		// open the map node:
		if (!m_MapNode->open())
		{
			OE_WARN << "Failed to open MapNode" << std::endl;
		}

		m_Map = m_MapNode->getMap();
	}

	_viewer = _pOsgWidget->getOsgViewer();
 	_viewer->setCameraManipulator(new osgEarth::EarthManipulator);

	m_rootNode->addChild(node.get());

	osgEarth::Util::SkyNode* skyNode = osgEarth::Util::SkyNode::create();
	skyNode->setName("SkyNode");
	osgEarth::DateTime time(2022, 1, 3, 12);
	skyNode->setDateTime(time);
	skyNode->setAtmosphereVisible(true);
	skyNode->setLighting(true);
	skyNode->attach(_viewer, 1);
	m_rootNode->addChild(skyNode);

	osgUtil::Optimizer optimizer;
	optimizer.optimize(m_rootNode.get());
	_viewer->setSceneData(m_rootNode.get());
	_viewer->realize();
}

void MainWindow::addCustomNode()
{
	using namespace osgEarth;
	using namespace osgEarth::Util;
	using namespace osgEarth::Contrib;
	using namespace osgEarth::Util;

	// find the map node that we loaded.
	MapNode* mapNode = MapNode::findMapNode(m_MapNode);
	if (!mapNode)
		return ;

	// Group to hold all our annotation elements.
	osg::Group* annoGroup = new osg::Group();
	m_MapNode->addChild(annoGroup);

	// Make a group for labels
	osg::Group* labelGroup = new osg::Group();
	annoGroup->addChild(labelGroup);

	osg::Group* editGroup = new osg::Group();
	m_MapNode->addChild(editGroup);

	// Style our labels:
	Style labelStyle;
	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_TOP;
	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;

	// A lat/long SRS for specifying points.
	const SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
	// A series of place nodes (an icon with a text label)
	{
		Style pm;
		pm.getOrCreate<IconSymbol>()->url()->setLiteral("./data/placemark32.png");
		pm.getOrCreate<IconSymbol>()->declutter() = true;
		pm.getOrCreate<TextSymbol>()->halo() = Color("#0f0f0f");

		// bunch of pins:
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, -74.00, 40.71), "New York", pm));
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, -77.04, 38.85), "Washington, DC", pm));
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, -118.40, 33.93), "Los Angeles", pm));
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, -71.03, 42.37), "Boston", pm));

		// test with an LOD:
		osg::LOD* lod = new osg::LOD();
		lod->addChild(new PlaceNode(GeoPoint(geoSRS, 14.68, 50.0), "Prague", pm), 50000, 2e6);
		labelGroup->addChild(lod);

		// absolute altitude:
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, -87.65, 41.90, 5000, ALTMODE_RELATIVE), "Chicago", pm));
	}

	// a box that follows lines of latitude (rhumb line interpolation, the default)
	// and flashes on and off using a cull callback.
	{
		struct C : public osg::NodeCallback {
			void operator()(osg::Node* n, osg::NodeVisitor* nv) {
				static int i = 0;
				i++;
				if (i % 100 < 50)
					traverse(n, nv);
			}
		};
		Geometry* geom = new osgEarth::Polygon();
		geom->push_back(osg::Vec3d(0, 40, 0));
		geom->push_back(osg::Vec3d(-60, 40, 0));
		geom->push_back(osg::Vec3d(-60, 60, 0));
		geom->push_back(osg::Vec3d(0, 60, 0));

		Feature* feature = new Feature(geom, geoSRS);
		feature->geoInterp() = GEOINTERP_RHUMB_LINE;

		Style geomStyle;
		geomStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::Cyan;
		geomStyle.getOrCreate<LineSymbol>()->stroke()->width() = 5.0f;
		geomStyle.getOrCreate<LineSymbol>()->tessellationSize()->set(75000, Units::METERS);
		geomStyle.getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;

		FeatureNode* fnode = new FeatureNode(feature, geomStyle);

		fnode->addCullCallback(new C());

		annoGroup->addChild(fnode);

		LabelNode* label = new LabelNode("Rhumb line polygon", labelStyle);
		label->setPosition(GeoPoint(geoSRS, -30, 50));
		labelGroup->addChild(label);
	}

}
