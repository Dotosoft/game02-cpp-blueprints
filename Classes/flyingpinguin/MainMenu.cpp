#include "MainMenu.h"
#include "GameWorld.h"
#include "Sky.h"
#include "CustomTerrain.h"

Scene* MainMenu::scene()
{
    Scene *scene = Scene::create();
    MainMenu *layer = MainMenu::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool MainMenu::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	CreateMenu();

	// add the hills
	CustomTerrain* terrain = CustomTerrain::create(NULL, 0);
	terrain->Update(Vec2(SCREEN_SIZE.width * 2.5f, SCREEN_SIZE.height * 0.5f));
	addChild(terrain, E_LAYER_FOREGROUND);

	// add the sky
	Sky* sky = Sky::create();
	addChild(sky, E_LAYER_BACKGROUND);

	return true;
}

void MainMenu::CreateMenu()
{
	Sprite* title = Sprite::create("title.png");
	title->setPosition(Vec2(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.65f));
	addChild(title, E_LAYER_HUD);

	ActionInterval* scale_up = EaseSineOut::create(ScaleTo::create(1.0f, 1.05f));
	ActionInterval* scale_down = EaseSineIn::create(ScaleTo::create(1.0f, 1.0f));
	title->runAction(RepeatForever::create(Sequence::createWithTwoActions(scale_up, scale_down)));

	// create & add the play button's menu
	Menu* menu = Menu::create();
	menu->setAnchorPoint(Point::ZERO);
	menu->setPosition(Point::ZERO);
	addChild(menu, E_LAYER_HUD);

	// create & add the play button
	MenuItemSprite* play_button = MenuItemSprite::create(Sprite::create("play.png"), Sprite::create("play.png"), this, menu_selector(MainMenu::OnPlayClicked));
	play_button->setPosition(Vec2(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.3f));
	menu->addChild(play_button);
}

void MainMenu::OnPlayClicked(Ref* sender)
{
	// handler function for the play button
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}
