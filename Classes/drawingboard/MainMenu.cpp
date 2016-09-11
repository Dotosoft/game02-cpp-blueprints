#include "MainMenu.h"
#include "BackgroundManager.h"
#include "GameWorld.h"
#include "ui/CocosGUI.h"

using namespace cocos2d::ui;

Scene* MainMenu::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainMenu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
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

	// enable accelerometer
	// setAccelerometerEnabled(true);
	Device::setAccelerometerEnabled(true);
	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(MainMenu::onAcceleration, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// create and add the stars
	background_ = BackgroundManager::create();
	addChild(background_, E_LAYER_BACKGROUND);

	// create & add the title of the game
	auto title = Sprite::createWithSpriteFrameName("iutitle");
	title->setPosition(Vec2(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * 1.2));
	title->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), EaseBackOut::create(MoveBy::create(0.5f, Vec2(0, SCREEN_SIZE.height * -0.5)))));
	addChild(title, E_LAYER_FOREGROUND);

	// create & add the play button's menu
	auto menu = Menu::create();
	menu->setAnchorPoint(Point::ZERO);
	menu->setPosition(Point::ZERO);
	addChild(menu, E_LAYER_FOREGROUND);

	// create & add the play button
	auto play_button = MenuItemSprite::create(Sprite::create("play_button.png"), Sprite::create("play_button.png"), this, menu_selector(MainMenu::OnPlayClicked));
	play_button->setColor(Color3B(77, 210, 245));
	play_button->setPosition(Vec2(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * -0.15));
	play_button->runAction(Sequence::createWithTwoActions(DelayTime::create(1.0f), EaseBackOut::create(MoveBy::create(0.5f, Vec2(0, SCREEN_SIZE.height * 0.5)))));
	menu->addChild(play_button);

	// checkbox
	auto checkbox = CheckBox::create("CheckBox_Normal.png",
		"CheckBox_Press.png",
		"CheckBoxNode_Normal.png",
		"CheckBox_Disable.png",
		"CheckBoxNode_Disable.png");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	checkbox->setPosition(Vec2(50, visibleSize.height - 50));
	checkbox->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		auto thisCheck = dynamic_cast<CheckBox*>(sender);
		switch (type)
		{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				UserDefault::getInstance()->setBoolForKey("useAccel", thisCheck->isSelected());
				break;
			default:
				break;
		}
	});

	auto useAccel = UserDefault::getInstance()->getBoolForKey("useAccel", false);
	checkbox->setSelected(useAccel);

	//add the menu item for back to main menu
	auto checkLabel = Label::createWithBMFont("infont.fnt", "Accelerometer");
	checkLabel->setPosition(checkbox->getPosition() + Vec2(checkLabel->getWidth() + 175, 0));
	addChild(checkbox);
	addChild(checkLabel);
    
    return true;
}

void MainMenu::onAcceleration(Acceleration* acc, Event* event)
{
	// move the stars according to the accelerometer
	background_->setPosition(Vec2(acc->x * -30, acc->y * -30));
}

void MainMenu::OnPlayClicked(Ref* sender)
{
	// handler function for the play button
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}
