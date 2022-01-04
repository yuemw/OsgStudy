// Osgearth02.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <iostream>

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <osgViewer/Viewer>   
#include <osg/Node>   
#include <osg/Geode>   
#include <osg/Group>   
#include <osgDB/ReadFile>   
#include <osgDB/WriteFile>   
#include <osgUtil/Optimizer>   
#include <osgEarth/Utils>
#include <osgEarth/MapNode>
#include <osgEarth/Sky>
#include <osgEarth/DateTime>


int main(int argc, _TCHAR* argv[])
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("simple.earth");

	if (nullptr == node)
		return 0;

	root->addChild(node.get());

	osgEarth::Util::SkyNode* skyNode = osgEarth::Util::SkyNode::create();
	skyNode->setName("SkyNode");
	osgEarth::DateTime time(2022,1,3,12);
	skyNode->setDateTime(time);
	skyNode->setAtmosphereVisible(true);
	skyNode->setLighting(true);
	skyNode->attach(viewer, 1);
	root->addChild(skyNode);

	osgUtil::Optimizer optimizer;
	optimizer.optimize(root.get());
	viewer->setSceneData(root.get());
	viewer->realize();
	viewer->run();

	return 0;
}