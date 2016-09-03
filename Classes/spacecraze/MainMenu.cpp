#include "spacecraze\MainMenu.h"
#include "spacecraze\BackgroundManager.h"
#include "spacecraze\GameWorld.h"

using namespace spacecraze;

CCScene* MainMenu::scene()
{
    // 'scene' is an autorelease object
    auto scene = CCScene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainMenu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool MainMenu::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

    // create the environment
	auto background_manager = BackgroundManager::create();
	addChild(background_manager, E_LAYER_BACKGROUND);

	// create & add the batch node
	sprite_batch_node_ = SpriteBatchNode::create(RESOURCES_SPACECRAZE_IMAGE_SPACETEX, 10);
	addChild(sprite_batch_node_, E_LAYER_FOREGROUND);

	// create & add the title of the game
	auto title = Sprite::createWithSpriteFrameName("sftitle");
	title->setPosition(ccp(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * 1.2));
	title->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), EaseBackOut::create(MoveBy::create(0.5f, ccp(0, SCREEN_SIZE.height * -0.5f)))));
	sprite_batch_node_->addChild(title, E_LAYER_FOREGROUND);

	// create & add the play button
	auto play_button = MenuItemSprite::create(Sprite::createWithSpriteFrameName("play"), Sprite::createWithSpriteFrameName("play"), this, menu_selector(MainMenu::OnPlayClicked));
	play_button->setPosition(ccp(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * -0.15));
	play_button->runAction(Sequence::createWithTwoActions(DelayTime::create(1.0f), EaseBackOut::create(MoveBy::create(0.5f, ccp(0, SCREEN_SIZE.height * 0.5f)))));

	// create & add the play button's menu
	auto play_menu = Menu::create(play_button, NULL);
	play_menu->setAnchorPoint(Point::ZERO);
	play_menu->setPosition(Point::ZERO);
	addChild(play_menu, E_LAYER_FOREGROUND);

	// time for some chaos!
	MainMenu::Animate(0.0f);
	MainMenu::Animate(0.0f);
	schedule(schedule_selector(MainMenu::Animate), 0.5f);

    return true;
}

void MainMenu::Animate(float dt)
{
	auto source = Point::ZERO;
	auto distance = Point::ZERO;

	// compute random duration of movement
	float duration = 1 + CCRANDOM_0_1() * 3;

	// choose between left-to-right or top-to-bottom
	if(CCRANDOM_MINUS1_1() > 0)
	{
		// left-to-right
		// compute starting position
		source.x = (CCRANDOM_MINUS1_1() > 0) ? (SCREEN_SIZE.width * -0.25f) : (SCREEN_SIZE.width * 1.25f);
		source.y = SCREEN_SIZE.height * 0.25f + CCRANDOM_0_1() * SCREEN_SIZE.height * 0.5f;

		// compute distance to travel
		distance.x = (source.x < 0) ? SCREEN_SIZE.width*1.5f : SCREEN_SIZE.width*-1.5f;
		distance.y = CCRANDOM_0_1() * SCREEN_SIZE.height;
	}
	else
	{
		// top-to-bottom
		// compute starting position
		source.x = SCREEN_SIZE.width * 0.25f + CCRANDOM_0_1() * SCREEN_SIZE.width * 0.5f;
		source.y = (CCRANDOM_MINUS1_1() > 0) ? (SCREEN_SIZE.height * -0.25f) : (SCREEN_SIZE.height * 1.25f);

		// compute distance to travel
		distance.x = CCRANDOM_0_1() * SCREEN_SIZE.width;
		distance.y = (source.y < 0) ? SCREEN_SIZE.height*1.5f : SCREEN_SIZE.height*-1.5f;
	}

	// choose an enemy ship
	char buf[16] = {0};
	int type = 1 + (int)(CCRANDOM_0_1() * 3);
	sprintf(buf, "sfenmy%d", type);

	// create enemy sprite & add it
	auto enemy_ship = Sprite::createWithSpriteFrameName(buf);
	sprite_batch_node_->addChild(enemy_ship);

	// position enemy sprite
	enemy_ship->setPosition(source);

	// create & run sequence of move & remove
	auto movement = MoveBy::create(duration, distance);
	auto remove = RemoveSelf::create(true);
	enemy_ship->runAction(Sequence::createWithTwoActions(movement, remove));
}

void MainMenu::OnPlayClicked(Ref* sender)
{
	// handler function for the play button
	CCDirector::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}
