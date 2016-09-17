#include "MainMenu.h"
#include "BackgroundManager.h"
#include "GameWorld.h"

Scene* MainMenu::scene()
{
    // 'scene' is an autorelease object
    Scene *scene = Scene::create();
    
    // 'layer' is an autorelease object
    MainMenu *layer = MainMenu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool MainMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	// create & add the BG
	Sprite* bg = Sprite::createWithSpriteFrameName("cjname.png");//create("cjname.png");
	bg->setPosition(Vec2(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.5f));
	addChild(bg);

	// create & add the title of the game
	Sprite* title = Sprite::createWithSpriteFrameName("cjname01.png");//create("cjname01.png");
	title->setPosition(Vec2(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.35f));
	addChild(title, E_LAYER_FOREGROUND);

	ActionInterval* move_up = EaseSineOut::create(ScaleTo::create(1.0f, 1.05f));
	ActionInterval* move_down = EaseSineIn::create(ScaleTo::create(1.0f, 1.0f));
	title->runAction(RepeatForever::create(Sequence::createWithTwoActions(move_up, move_down)));

	// create & add the play button
	MenuItemSprite* play_button = MenuItemSprite::create(Sprite::createWithSpriteFrameName("cjbplay.png"), Sprite::createWithSpriteFrameName("cjbplay.png"), this, menu_selector(MainMenu::OnPlayClicked));
	play_button->setPosition(Vec2(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.05f));

	// create & add the play button's menu
	Menu* play_menu = Menu::create(play_button, NULL);
	play_menu->setAnchorPoint(Point::ZERO);
	play_menu->setPosition(Point::ZERO);
	addChild(play_menu, E_LAYER_FOREGROUND);

    return true;
}

void MainMenu::OnPlayClicked(Ref* sender)
{
	// handler function for the play button
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}
