#include "MainMenu.h"
#include "GameWorld.h"

Scene* MainMenu::scene()
{
    Scene *scene = Scene::create();
    MainMenu *layer = MainMenu::create();
    scene->addChild(layer);
    return scene;
}

bool MainMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	
	LayerColor* bg = LayerColor::create(Color4B(25, 49, 69, 255));
	addChild(bg);

	TMXTiledMap* tiled_map = TMXTiledMap::create("level_01.tmx");
	addChild(tiled_map);

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
	addChild(menu);

	// create & add the play button
	MenuItemSprite* play_button = MenuItemSprite::create(Sprite::create("play_button.png"), Sprite::create("play_button.png"), this, menu_selector(MainMenu::OnPlayClicked));
	play_button->setPosition(Vec2(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.35f));
	play_button->setScale(1.5f);
	menu->addChild(play_button);

	return true;
}

void MainMenu::OnPlayClicked(Ref* sender)
{
	// handler function for the play button
	// start the player off with 3 lives
	GameGlobals::level_number_ = 1;
	GameGlobals::hero_lives_left_ = 3;
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}