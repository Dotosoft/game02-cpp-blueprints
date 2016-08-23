#include "colorsmasher\MainMenuScene.h"

Scene* MainMenu::createScene()
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
    
    auto screenSize = Director::getInstance()->getWinSize();

	auto tileWidth = Sprite::create("tile.png")->getContentSize().width;
	float borderOffsetCalc = screenSize.width * BORDER_OFFSET;
	this->TILE_SIZE = (screenSize.width - borderOffsetCalc) / NUM_COLS;
	this->GAMEPLAY_OFFSET = Vec2(borderOffsetCalc / 2, borderOffsetCalc);
	this->tileZoomFactor = TILE_SIZE / tileWidth;

    // create a coloured layer as background
    auto background = LayerColor::create( Color4B( 25, 0, 51, 255 ), screenSize.width, screenSize.height );
    this->addChild(background);

    auto *titleLabel = Label::createWithTTF("ColourSmash",RESOURCES_FONT_COMIC_SANS, 32);
    titleLabel->setPosition(Vec2(screenSize.width * 0.5, screenSize.height * 0.8));
    this->addChild(titleLabel, 1);

    auto playButtonNormal = Sprite::create(RESOURCES_IMAGE_BUTTON_PLAY);
    auto playButtonClicked = Sprite::create(RESOURCES_IMAGE_BUTTON_PLAY);
    auto playButton = MenuItemSprite::create(playButtonNormal, playButtonClicked, this, menu_selector(MainMenu::onPlayClicked) );
    playButton->setPosition(Vec2(screenSize.width * 0.5, screenSize.height * 0.5));

    // create a menu that will contain the button above
    auto menu = Menu::create(playButton, NULL);
    menu->setPosition(Vec2(0,0));
    this->addChild(menu, 1);

	this->doAnimation(2.0f);
	this->schedule(schedule_selector(MainMenu::doAnimation), 2.0f);

    return true;
}

void MainMenu::doAnimation(float dt) {
	int numTiles = random(0, 30);
	for (int i = 0; i < numTiles; ++i)
	{
		auto tile = Sprite::create("tile.png");
		tile->setColor(this->getColourForTile(E_COLOUR_TYPE(random(1, int(MAX_COLOURS)))));
		tile->setPosition(this->getRandomPositionForTile());
		tile->setScale(0);
		this->addChild(tile);

		auto waitBefore = DelayTime::create(CCRANDOM_0_1() * 5);
		auto scaleUp = EaseBackOut::create(ScaleTo::create(0.125f, this->tileZoomFactor));
		auto waitAfter = DelayTime::create(CCRANDOM_0_1() * 5);
		auto scaleDown = EaseBackIn::create(ScaleTo::create(0.125, 0));
		auto removeSelf = RemoveSelf::create(true);
		tile->runAction(Sequence::create(waitBefore, scaleUp, waitAfter, scaleDown, removeSelf, NULL));
	}
}

Vec2 MainMenu::getRandomPositionForTile() {
	return Vec2(random(0, NUM_COLS) * 25.0f, random(0, NUM_ROWS + 5) * 25.0f);
}

Color3B MainMenu::getColourForTile(E_COLOUR_TYPE colourData) {
	// CCLOG("getColourForTile");
	// return a colour for the specified type
	auto colorResult = Color3B::WHITE;
	switch (colourData)
	{
	case E_COLOUR_RED:
		colorResult = Color3B(255, 128, 0);
		break;
	case E_COLOUR_GREEN:
		colorResult = Color3B(0, 204, 0);
		break;
	case E_COLOUR_BLUE:
		colorResult = Color3B(0, 76, 153);
		break;
	case E_COLOUR_YELLOW:
		colorResult = Color3B(102, 0, 102);
		break;
	}

	return colorResult;
}

void MainMenu::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
	#endif
}

void MainMenu::onPlayClicked(Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::createScene()));
}
