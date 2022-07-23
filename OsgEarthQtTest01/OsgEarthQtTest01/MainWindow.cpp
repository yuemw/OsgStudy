#include "MainWindow.h"
#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QTimer>
 
#include <osg/Node>   
#include <osg/Geode>   
#include <osg/Group>   
#include <osg/Array>
#include <osg/Matrixd>
#include <osgDB/WriteFile>  
#include <osgUtil/Optimizer>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

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
#include <osgEarth/GLUtils>
#include <osgEarth/LatLongFormatter>
#include <osgEarth/GeodeticGraticule>
#include <osgEarth/MouseCoordsTool>
#include <osgEarth/ModelNode>
#include <osgEarth/GeoMath>

#include "MyPlanPathCallback.h"

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace osgEarth::Contrib;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	this->setMinimumSize(1024, 768);

	ui.lineEdit_file->setEnabled(false);

	_pOsgWidget = new osgQOpenGLWidget(this);
	
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(_pOsgWidget);
	ui.widget_2->setLayout(layout);

	connect(_pOsgWidget, SIGNAL(initialize()), this, SLOT(initOsgWindow()));
}

void MainWindow::init()
{
	QTimer::singleShot(100, this, [&]() {
		initOsgEarthWindow();
	});
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
//	initOsgEarthWindow();
	addGraticule();
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
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("F:/work/Project/MutilObject/bin/Data/map/3d.earth");//mymap.earth
	if (nullptr == node)
		return;

	if (node.valid())
	{
		m_mapNode = osgEarth::MapNode::get(node.get());
		// open the map node:
		if (!m_mapNode->open())
		{
			OE_WARN << "Failed to open MapNode" << std::endl;
		}
	}

	_viewer = _pOsgWidget->getOsgViewer();
	m_em = new osgEarth::EarthManipulator;
 	_viewer->setCameraManipulator(m_em);

	// initialize the top level state
	GLUtils::setGlobalDefaults(_viewer->getCamera()->getOrCreateStateSet());

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

	// disable the small-feature culling
	_viewer->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

	// set a near/far ratio that is smaller than the default. This allows us to get
	// closer to the ground without near clipping. If you need more, use --logdepth
	_viewer->getCamera()->setNearFarRatio(0.0001);

	// finalize setup and run.
	_viewer->addEventHandler(new osgViewer::StatsHandler());
	_viewer->addEventHandler(new osgViewer::WindowSizeHandler());
	_viewer->addEventHandler(new osgViewer::ThreadingHandler());
	_viewer->addEventHandler(new osgGA::StateSetManipulator(_viewer->getCamera()->getOrCreateStateSet()));
	_viewer->realize();
}

//添加经纬度线
void MainWindow::addGraticule()
{
	Formatter* formatter = 0L;
	{
		GeodeticGraticule* gr = new GeodeticGraticule();
		m_mapNode->getMap()->addLayer(gr);
		formatter = new LatLongFormatter();
	}

	// mouse coordinate readout:
	ControlCanvas* canvas = new ControlCanvas();
	m_rootNode->addChild(canvas);
	VBox* vbox = new VBox();
	canvas->addControl(vbox);

	LabelControl* readout = new LabelControl();
	vbox->addControl(readout);

	MouseCoordsTool* tool = new MouseCoordsTool(m_mapNode);
	tool->addCallback(new MouseCoordsLabelCallback(readout, formatter));
	_viewer->addEventHandler(tool);
}

void MainWindow::addCustomNode()
{
	using namespace osgEarth;
	using namespace osgEarth::Util;
	using namespace osgEarth::Contrib;
	using namespace osgEarth::Util;

	// find the map node that we loaded.
// 	MapNode* mapNode = MapNode::findMapNode(m_MapNode);
// 	if (!mapNode)
// 		return ;

	// Group to hold all our annotation elements.
	osg::Group* annoGroup = new osg::Group();
	m_mapNode->addChild(annoGroup);

	// Make a group for labels
	osg::Group* labelGroup = new osg::Group();
	annoGroup->addChild(labelGroup);

	osg::Group* editGroup = new osg::Group();
	m_mapNode->addChild(editGroup);

	// Style our labels:
	Style labelStyle;
	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_TOP;
	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;

	// A lat/long SRS for specifying points.
	const SpatialReference* geoSRS = m_mapNode->getMapSRS()->getGeographicSRS();
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
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, 116.23, 39.54), "Beijing", pm));

		// test with an LOD:
		osg::LOD* lod = new osg::LOD();
		lod->addChild(new PlaceNode(GeoPoint(geoSRS, 14.68, 50.0), "Prague", pm), 50000, 2e6);
		labelGroup->addChild(lod);

		// absolute altitude:
		labelGroup->addChild(new PlaceNode(GeoPoint(geoSRS, -87.65, 41.90, 5000, ALTMODE_RELATIVE), "Chicago", pm));
	}

	// A path using great-circle interpolation.
	// Keep a pointer to it so we can modify it later on.
	FeatureNode* pathNode = 0;
	{
		osgEarth::Geometry* path = new LineString();
		path->push_back(osg::Vec3d(-74, 40.714, 0));   // New York
		path->push_back(osg::Vec3d(116.23, 39.54, 0)); // Tokyo

		Feature* pathFeature = new Feature(path, geoSRS);
		pathFeature->geoInterp() = GEOINTERP_RHUMB_LINE;

		Style pathStyle;
		pathStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::White;
		pathStyle.getOrCreate<LineSymbol>()->stroke()->width() = 1.0f;
		pathStyle.getOrCreate<LineSymbol>()->stroke()->smooth() = true;
		pathStyle.getOrCreate<LineSymbol>()->tessellationSize()->set(75000, Units::METERS);
		pathStyle.getOrCreate<PointSymbol>()->size() = 8;
		pathStyle.getOrCreate<PointSymbol>()->fill()->color() = Color::Red;
		pathStyle.getOrCreate<PointSymbol>()->smooth() = true;
		pathStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		pathStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_GPU;
		pathStyle.getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;

		//OE_INFO << "Path extent = " << pathFeature->getExtent().toString() << std::endl;

		pathNode = new FeatureNode(pathFeature, pathStyle);
		annoGroup->addChild(pathNode);

		LabelNode* label = new LabelNode("Great circle path", labelStyle);
		label->setPosition(GeoPoint(geoSRS, -170, 61.2));
		labelGroup->addChild(label);
	}
	//--------------------------------------------------------------------

	// Two circle segments around New Orleans.
	{
		Style circleStyle;
		circleStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Cyan, 0.5);
		circleStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		circleStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;

		CircleNode* circle = new CircleNode();
		circle->set(GeoPoint(geoSRS, -90.25, 29.98, 1000., ALTMODE_RELATIVE),
			Distance(300, Units::KILOMETERS),
			circleStyle,
			Angle(-45.0, Units::DEGREES),
			Angle(45.0, Units::DEGREES),
			true);

		annoGroup->addChild(circle);
	}
	{
		Style circleStyle;
		circleStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Red, 0.5);
		circleStyle.getOrCreate<ExtrusionSymbol>()->height() = 250000.0;
		circleStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		circleStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;

		CircleNode* circle = new CircleNode();
		circle->set(
			GeoPoint(geoSRS, -90.25, 29.98, 1000., ALTMODE_RELATIVE),
			Distance(300, Units::KILOMETERS),
			circleStyle,
			Angle(45.0, Units::DEGREES),
			Angle(360.0 - 45.0, Units::DEGREES),
			true);

		annoGroup->addChild(circle);
	}
	//--------------------------------------------------------------------

	// An extruded ellipse around Miami.
	{
		Style ellipseStyle;
		ellipseStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Orange, 0.75);
		ellipseStyle.getOrCreate<ExtrusionSymbol>()->height() = 250000.0; // meters MSL
		EllipseNode* ellipse = new EllipseNode();
		ellipse->set(
			GeoPoint(geoSRS, -80.28, 25.82, 0.0, ALTMODE_RELATIVE),
			Distance(250, Units::MILES),
			Distance(100, Units::MILES),
			Angle(0, Units::DEGREES),
			ellipseStyle,
			Angle(45.0, Units::DEGREES),
			Angle(360.0 - 45.0, Units::DEGREES),
			true);
		annoGroup->addChild(ellipse);
	}
	{
		Style ellipseStyle;
		ellipseStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Blue, 0.75);
		ellipseStyle.getOrCreate<ExtrusionSymbol>()->height() = 250000.0; // meters MSL
		EllipseNode* ellipse = new EllipseNode();
		ellipse->set(
			GeoPoint(geoSRS, -80.28, 25.82, 0.0, ALTMODE_RELATIVE),
			Distance(250, Units::MILES),
			Distance(100, Units::MILES),
			Angle(0, Units::DEGREES),
			ellipseStyle,
			Angle(-40.0, Units::DEGREES),
			Angle(40.0, Units::DEGREES),
			true);
		annoGroup->addChild(ellipse);
	}

	//--------------------------------------------------------------------

	{
		// A rectangle around San Diego
		Style rectStyle;
		rectStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Green, 0.5);
		rectStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;
		rectStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
		rectStyle.getOrCreate<ExtrusionSymbol>()->height() = 200000.0; // meters MSL
		RectangleNode* rect = new RectangleNode(
			GeoPoint(geoSRS, -85.28, 25.82, 0.0),
			Distance(300, Units::KILOMETERS),
			Distance(600, Units::KILOMETERS),
			rectStyle);
		annoGroup->addChild(rect);
	}

	//--------------------------------------------------------------------
		// An extruded polygon roughly the shape of Utah. Here we demonstrate the
	// FeatureNode, where you create a geographic geometry and use it as an
	// annotation.
	{
		osgEarth::Geometry* utah = new osgEarth::Polygon();
		utah->push_back(-114.052, 37.0);
		utah->push_back(-109.054, 37.0);
		utah->push_back(-109.054, 41.0);
		utah->push_back(-111.040, 41.0);
		utah->push_back(-111.080, 42.059);
		utah->push_back(-114.080, 42.024);

		Style utahStyle;
		utahStyle.getOrCreate<ExtrusionSymbol>()->height() = 250000.0; // meters MSL
		utahStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::White, 0.8);

		Feature*     utahFeature = new Feature(utah, geoSRS);
		FeatureNode* featureNode = new FeatureNode(utahFeature, utahStyle);

		annoGroup->addChild(featureNode);
	}
	//--------------------------------------------------------------------

	// an image overlay.
	ImageOverlay* imageOverlay = 0L;
	osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile("./data/USFLAG.TGA");
	if (image.valid())
	{
		imageOverlay = new ImageOverlay(m_mapNode, image.get());
		imageOverlay->setBounds(Bounds(-100.0, 35.0, -90.0, 40.0));

		annoGroup->addChild(imageOverlay);

		editGroup->addChild(new ImageOverlayEditor(imageOverlay));
	}
	//--------------------------------------------------------------------

	// a model node with auto scaling.
	{
		Style style;
		style.getOrCreate<ModelSymbol>()->autoScale() = true;
		style.getOrCreate<ModelSymbol>()->url()->setLiteral("./data/red_flag.osg.50.scale");
		ModelNode* modelNode = new ModelNode(m_mapNode, style);
		modelNode->setPosition(GeoPoint(geoSRS, -100, 52, 100));
		annoGroup->addChild(modelNode);
	}
	//--------------------------------------------------------------------

	//Load a plane model.
	addPlane();

	osg::ref_ptr<osg::Node> plane = osgDB::readRefNodeFile("./data/cessna.osgb.500,500,500.scale");
	//Create 2 moving planes
// 	osg::Node* plane1 = createPlane(plane.get(), GeoPoint(geoSRS, -100.1, 52, 5000, ALTMODE_ABSOLUTE), geoSRS, 100000, 20);
// 	m_rootNode->addChild(plane1);
// 	osg::Node* plane2 = createPlane(plane.get(), GeoPoint(geoSRS, -101.321, 51.2589, 3390.09, ALTMODE_ABSOLUTE), geoSRS, 30000, 30);
// 	m_rootNode->addChild(plane2);

	//	startTrackNode(plane1);
}
osg::Node* MainWindow::createPlane(osg::Node* node, const GeoPoint& pos, const SpatialReference* mapSRS, double radius, double time)
{
	osg::MatrixTransform* positioner = new osg::MatrixTransform;
	positioner->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);
	positioner->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	positioner->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	positioner->addChild(node);
	osg::AnimationPath* animationPath = createAnimationPath(pos, mapSRS, radius, time);
	positioner->setUpdateCallback(new MyPlanPathCallback(animationPath, 0.0, 1.0));
	return positioner;
}

osg::AnimationPath* MainWindow::createAnimationPath(const GeoPoint& pos, const SpatialReference* mapSRS, float radius, double looptime)
{
	// set up the animation path
	osg::AnimationPath* animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);

	int numSamples = 40;

	double delta = osg::PI * 2.0 / (double)numSamples;

	//Get the center point in geocentric
	GeoPoint mapPos = pos.transform(mapSRS);
	osg::Vec3d centerWorld;
	mapPos.toWorld(centerWorld);

	bool isProjected = mapSRS->isProjected();

	osg::Vec3d up = isProjected ? osg::Vec3d(0, 0, 1) : centerWorld;
	up.normalize();

	//Get the "side" vector
	osg::Vec3d side = isProjected ? osg::Vec3d(1, 0, 0) : up ^ osg::Vec3d(0, 0, 1);


	double time = 0.0f;
	double time_delta = looptime / (double)numSamples;

	osg::Vec3d firstPosition;
	osg::Quat firstRotation;

	for (unsigned int i = 0; i < (unsigned int)numSamples; i++)
	{
		double angle = delta * (double)i;
		osg::Quat quat(angle, up);
		osg::Vec3d spoke = quat * (side * radius);
		osg::Vec3d end = centerWorld + spoke;

		osg::Quat makeUp;
		makeUp.makeRotate(osg::Vec3d(0.3, 0, 1), up);

		osg::Quat rot = makeUp;
		animationPath->insert(time, osg::AnimationPath::ControlPoint(end, rot));
		if (i == 0)
		{
			firstPosition = end;
			firstRotation = rot;
		}
		time += time_delta;
	}

	animationPath->insert(time, osg::AnimationPath::ControlPoint(firstPosition, firstRotation));

	return animationPath;
}

void MainWindow::addPlane()
{
#if 1
	osg::ref_ptr<CoordinateSystemNode> csn = new CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel());

	osg::ref_ptr<osg::Node> plane = osgDB::readNodeFile("./data/J10.ive.200,200,200.scale");
	if (!plane.valid())
		return;

	osg::MatrixTransform* mtFlySelf = new osg::MatrixTransform;
	mtFlySelf->setMatrix(/*osg::Matrixd::scale(2, 2, 2)*/osg::Matrixd::rotate(45, osg::Vec3d(0,0,1)));
	mtFlySelf->addChild(plane);

	osg::MatrixTransform* mtFly = new osg::MatrixTransform;
	mtFly->addChild(mtFlySelf);
	m_rootNode->addChild(mtFly);

	osg::Matrixd mtTemp;
	csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(
		osg::DegreesToRadians(52.0),
		osg::DegreesToRadians(-100.3),
		5000,
		mtTemp);

	mtFly->setMatrix(mtTemp);

	mtFly->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);
	mtFly->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	mtFly->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	mtFly->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::AnimationPath* path = createPlanePath();
	mtFly->setUpdateCallback(new osg::AnimationPathCallback(path, 0.0, 1.0));

	startTrackNode(plane);

	addTail(osg::Vec3(0, -400, 0), mtFly);
	addTail(osg::Vec3(0, 400, 0), mtFly);


#else
	//添加模型
	osg::ref_ptr<osg::Node> nodeFile = osgDB::readNodeFile("./data/J10.ive.1000,1000,1000.scale");
	if (!nodeFile.valid())
	{
		return;
	}
	osg::ref_ptr<osg::MatrixTransform> nodeMt = new osg::MatrixTransform;//平移矩阵

	osg::ref_ptr<osg::MatrixTransform> nodeMtR = new osg::MatrixTransform;//旋转矩阵
	nodeMt->addChild(nodeMtR);
	osg::ref_ptr<osg::MatrixTransform> nodeMtS = new osg::MatrixTransform;//缩放矩阵
	nodeMtR->addChild(nodeMtS);
	nodeMtS->addChild(nodeFile);

	m_mapNode->addChild(nodeMt);

	osg::Matrix m;
	m_mapNode->getMapSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(
		osg::DegreesToRadians(52.0),
		osg::DegreesToRadians(-100.3),
		30000,
		m);

	nodeMt->setMatrix(m);
	nodeMt->addDescription("model-3d");

	nodeMt->getOrCreateStateSet()->setMode(GL_LIGHT1, osg::StateAttribute::ON);
	nodeMt->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	nodeMt->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	nodeMt->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	//设置模型显示比例
	nodeMtS->setMatrix(osg::Matrix::scale(5, 5, 5));
#endif
}

void MainWindow::startTrackNode(osg::Node* node)
{
	osgEarth::Viewpoint vp = m_em->getViewpoint();
	vp.setNode(node);
	vp.range()->set(200000.0, osgEarth::Units::METERS);//观察的距离
	vp.pitch()->set(-120.0, osgEarth::Units::DEGREES);//观察的角度
	m_em->setViewpoint(vp, 1.0);
}

osg::AnimationPath* MainWindow::createPlanePath()
{
#if 0
	const SpatialReference* mapSRS = m_mapNode->getMapSRS()->getGeographicSRS();
	GeoPoint pos(mapSRS, -100.36, 52.1, 5000, ALTMODE_ABSOLUTE);
	double looptime = 40.0;
	double radius = 100000;

	// set up the animation path
	osg::AnimationPath* animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);

	int numSamples = 40;

	double delta = osg::PI * 2.0 / (double)numSamples;

	//Get the center point in geocentric
	GeoPoint mapPos = pos.transform(mapSRS);
	osg::Vec3d centerWorld;
	mapPos.toWorld(centerWorld);

	bool isProjected = mapSRS->isProjected();

	osg::Vec3d up = isProjected ? osg::Vec3d(0, 0, 1) : centerWorld;
	up.normalize();

	//Get the "side" vector
	osg::Vec3d side = isProjected ? osg::Vec3d(1, 0, 0) : up ^ osg::Vec3d(0, 0, 1);

	double time = 0.0f;
	double time_delta = looptime / (double)numSamples;

	osg::Vec3d firstPosition;
	osg::Quat firstRotation;

	double len = radius / looptime;
	for (unsigned int i = 0; i < (unsigned int)numSamples; i++)
	{
// 		double angle = delta * (double)i;
// 		osg::Quat quat(angle, up);
// 		osg::Vec3d spoke = quat * (side * radius);
// 		osg::Vec3d end = centerWorld + spoke;

		double nextLon, nextLat;
		osgEarth::GeoMath::destination(osg::DegreesToRadians(mapPos.y()), osg::DegreesToRadians(mapPos.x()),
			60, len * i, nextLat, nextLon);
		osg::Vec3d end(osg::RadiansToDegrees(nextLon), osg::RadiansToDegrees(nextLat), 5000);

		GeoPoint tmp = GeoPoint(mapSRS, end, ALTMODE_ABSOLUTE).transform(mapSRS);
		tmp.toWorld(end);

		osg::Quat makeUp;
		makeUp.makeRotate(osg::Vec3d(0, 0, 30), end);

		osg::Quat rot = makeUp;
		animationPath->insert(time, osg::AnimationPath::ControlPoint(end, rot));
		if (i == 0)
		{
			firstPosition = end;
			firstRotation = rot;
		}
		time += time_delta;
	}

	animationPath->insert(time, osg::AnimationPath::ControlPoint(firstPosition, firstRotation));

	return animationPath;
#else
	const SpatialReference* mapSRS = m_mapNode->getMapSRS()->getGeographicSRS();

	osg::ref_ptr<osg::Vec4Array> points = new osg::Vec4Array();
	points->push_back(osg::Vec4(-100.3321, 52.0, 1000, 200));
	points->push_back(osg::Vec4(-100.34, 52.05, 2000, 500));
	points->push_back(osg::Vec4(-100.41, 52.10, 3000, 1000));
	points->push_back(osg::Vec4(-100.53, 52.32, 5000, 1500));
	points->push_back(osg::Vec4(-101.13, 52.43, 5000, 1500));

	osg::AnimationPath* animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double horizonAngle = 0.0;
	double verticalAngle = 0.0;
	double time = 0.0;

	osg::Matrix matrix;
	osg::Quat _roration;

	osg::Vec3d curPosition;
	osg::Vec3d nextPosition;

	for (osg::Vec4Array::iterator iter = points->begin(); iter != points->end(); ++iter)
	{
		osg::Vec4Array::iterator iter2 = iter;
		++iter2;

		//iter2 is end
		if (iter2 == points->end())
			break;

		double x, y, z;
		mapSRS->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter->y()),
			osg::DegreesToRadians(iter->x()), iter->z(), x, y, z);
		curPosition = osg::Vec3d(x, y, z);
		mapSRS->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(iter2->y()),
			osg::DegreesToRadians(iter2->x()), iter2->z(), x, y, z);
		nextPosition = osg::Vec3d(x, y, z);

		// 航向角
		horizonAngle = osgEarth::GeoMath::bearing(osg::DegreesToRadians(iter->y()), osg::DegreesToRadians(iter->x()),
			osg::DegreesToRadians(iter2->y()), osg::DegreesToRadians(iter2->x()));

		//俯仰角
		if (iter->z() == iter2->z())
		{
			verticalAngle = 0.0;
		}
		else
		{
			if (0 == sqrt(pow(getDistance(curPosition, nextPosition), 2) - pow((iter->z() - iter2->z()), 2)))
			{
				verticalAngle = osg::PI_2;
			}
			else
			{
				verticalAngle = atan((iter2->z() - iter->z()) / sqrt(pow(getDistance(curPosition, nextPosition), 2) - pow((iter->z() - iter2->z()), 2)));
			}
			if (verticalAngle >= osg::PI_2)
				verticalAngle = osg::PI_2;
			if (verticalAngle <= -osg::PI_2)
				verticalAngle = -osg::PI_2;

		}
		// 求飞机的变换矩阵
		mapSRS->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(iter->y()),
			osg::DegreesToRadians(iter->x()), iter->z(), matrix);

		_roration.makeRotate(0, osg::Vec3(1, 0, 0), verticalAngle/* + osg::PI_2*/, 
			osg::Vec3(0, 1, 0), horizonAngle, osg::Vec3(0, 0, 1));
		matrix.preMultRotate(_roration);

		animationPath->insert(time, osg::AnimationPath::ControlPoint(curPosition, matrix.getRotate()));

		// 求下一个点的时间
		time += getRunTime(curPosition, nextPosition, iter2->w());
	}

	animationPath->insert(time, osg::AnimationPath::ControlPoint(nextPosition, matrix.getRotate()));
	return animationPath;
#endif
}

double MainWindow::getDistance(osg::Vec3d pos1, osg::Vec3d pos2)
{
	double dist = sqrt(pow((pos1.x() - pos2.x()), 2) + pow((pos1.y() - pos2.y()), 2) + pow((pos1.z() - pos2.z()), 2));
	return dist;
}

double MainWindow::getRunTime(osg::Vec3 from, osg::Vec3 to, double speed)
{
	if (0 == speed)
		return 1000000;

	return getDistance(from, to) / speed;
}

#include <osgParticle/FireEffect>
void MainWindow::addTail(osg::Vec3 position, osg::MatrixTransform* scale)
{
	osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position, 100);
	fire->setUseLocalParticleSystem(false);
	fire->getEmitter()->setEndless(true);
	fire->getEmitter()->setLifeTime(1);
	fire->getParticleSystem()->getDefaultParticleTemplate().setLifeTime(fire->getParticleSystem()->getDefaultParticleTemplate().getLifeTime() * 10);

	scale->addChild(fire);

	osg::ref_ptr<Geode> geode = new osg::Geode;
	geode->addDrawable(fire->getParticleSystem());
	m_rootNode->addChild(geode);
}

