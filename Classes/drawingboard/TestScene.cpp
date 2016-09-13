#include "TestScene.h"
#include "ui/CocosGUI.h"

using namespace cocos2d::ui;

#define NUM_SPIKES 10

Scene* TestScene::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = TestScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool TestScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	auto drawNode = DrawNode::create();

	vector<Point> inner_vertices, outer_vertices;
	// get two regular polygons, one smaller than the other and with a slightly advance rotation
	// log("\ninner_vertices");
	GameGlobals::GetRegularPolygonVertices(inner_vertices, NUM_SPIKES, 50);
	// log("\nouter_vertices");
	GameGlobals::GetRegularPolygonVertices(outer_vertices, NUM_SPIKES, 50 * 1.5f, M_PI / NUM_SPIKES);

	Point verticesA[NUM_SPIKES];
	std::copy(inner_vertices.begin(), inner_vertices.end(), verticesA);

	Point verticesB[NUM_SPIKES];
	std::copy(outer_vertices.begin(), outer_vertices.end(), verticesB);

	drawNode->drawPoly(verticesA, NUM_SPIKES, false, Color4F(1, 0, 0, 1));
	// drawNode->drawPolygon(vertices, NUM_SPIKES, Color4F(0, 0, 0, 0), 1.5f, Color4F(1, 0, 0, 1));
	drawNode->drawPoly(verticesB, NUM_SPIKES, false, Color4F(1, 0, 0, 1));

	drawNode->setPosition(Director::getInstance()->getWinSize().width / 2, Director::getInstance()->getWinSize().height / 2);

	addChild(drawNode, 4);
    
    return true;
}