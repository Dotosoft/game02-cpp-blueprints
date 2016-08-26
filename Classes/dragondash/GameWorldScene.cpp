#include "dragondash\GameWorldScene.h"
using namespace dragondash;

Scene* GameWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameWorld::init()
{
    //////////////////////////////
    // 1. super init first
	if ( !Layer::init() )
    {
        return false;
    }

	// this._super();
	// enable touch
	this->setTouchEnabled(true);
	// store screen size for fast access
	this->screenSize = Director::getInstance()->getWinSize();

	// create and add the batch node
	this->spriteBatchNode = SpriteBatchNode::create(RESOURCES_IMAGE_SHEET_DRAGON, 256);
	this->addChild(this->spriteBatchNode, E_ZORDER::E_LAYER_BG + 1);

	// set the roof of the castle
	this->castleRoof = 100;
	// create & init all managers
	this->towerManager = new TowerManager(this);
	this->towerManager->init();
	this->dragonManager = new DragonManager(this);
	this->dragonManager->init();
	this->fairytaleManager = new FairyTaleManager(this);
	this->fairytaleManager->init();
	this->createHUD();

	this->scheduleUpdate();

	this->listener = EventListenerTouchOneByOne::create();
	this->listener->onTouchBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void GameWorld::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void GameWorld::createHUD()
{
	this->score = 0;
	// create labels for score
	this->scoreLabel = Label::createWithTTF(__String::createWithFormat("%d", this->score)->getCString(), RESOURCES_FONT_COMIC_SANS, 120);
	this->scoreLabel->setPosition(Vec2(this->screenSize.width * 0.5, this->screenSize.height * 0.875));
	this->addChild(this->scoreLabel, E_ZORDER::E_LAYER_HUD);

	// create the tutorial sprite and add it to the batch node
	this->tutorialSprite = Sprite::createWithSpriteFrameName("dhtap");
	this->tutorialSprite->setPosition(Vec2(this->screenSize.width * 0.5, this->screenSize.height * 0.5));
	this->spriteBatchNode->addChild(this->tutorialSprite);
}

void GameWorld::update(float dt)
{
	// update dragon
	this->dragonManager->update(dt);
	// update towers only after game has started
	if (this->hasGameStarted) 
	{
		this->towerManager->update();
	}
	// update environment
	this->fairytaleManager->update();
	this->checkCollisions();
}

void GameWorld::checkCollisions()
{
	// first find out which tower is right in front
	auto frontTower = this->towerManager->getFrontTower();

	// fetch the bounding boxes of the respective sprites
	auto dragonAABB = this->dragonManager->dragonSprite->getBoundingBox();
	auto lowerTowerAABB = frontTower->lowerSprite->getBoundingBox();
	auto upperTowerAABB = frontTower->upperSprite->getBoundingBox();

	// if the respective rects intersect, we have a collision
	if (dragonAABB.intersectsRect(lowerTowerAABB) || dragonAABB.intersectsRect(upperTowerAABB))
	{
		// dragon must die
		this->dragonManager->dragonDeath();
		// stop the update loop
		this->unscheduleUpdate();
	}
	else if (abs(lowerTowerAABB.getMidX() - dragonAABB.getMidX()) <= MAX_SCROLLING_SPEED / 2)
	{
		// increment score once the dragon has crossed the tower
		this->incrementScore();
	}
}

void GameWorld::incrementScore()
{
	++this->score;
	this->scoreLabel->setString(this->score + "");
	// run a simple action so the user knows the score is being added
	// use the ease functions to create a heart beat effect
	this->scoreLabel->runAction(Sequence::create(EaseSineIn::create(ScaleTo::create(0.125, 1.2)), EaseSineOut::create(ScaleTo::create(0.125, 1)), NULL));
}

void GameWorld::onGameOver(Node *node)
{
	this->showGameOverPopup();
}

bool GameWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
	this->hasGameStarted = true;
	// remove the tutorial only if it exists
	if (this->tutorialSprite)
	{
		// fade it out and then remove it
		this->tutorialSprite->runAction(Sequence::create(FadeOut::create(0.25), RemoveSelf::create(true), NULL));
		this->tutorialSprite = NULL;
	}
	// inform DragonManager that the game has started
	this->dragonManager->onGameStart();
	// fly dragon...fly!!!
	this->dragonManager->dragonFlap();

	return true;
}

void GameWorld::showGameOverPopup()
{
	this->popup = LayerColor::create(Color4B(0, 0, 0, 196), this->screenSize.width, this->screenSize.height);
	this->popup->setOpacity(0);
	this->popup->runAction(FadeTo::create(0.25, 196));
	this->addChild(this->popup, E_ZORDER::E_LAYER_POPUPS);

	auto restartSprite = Sprite::createWithSpriteFrameName("dhplay");
	auto restartSpriteSelected = Sprite::createWithSpriteFrameName("dhplay");
	auto restartButton = MenuItemSprite::create(restartSprite, restartSpriteSelected, this, menu_selector(GameWorld::onRestartClicked));
	restartButton->setPosition(Vec2(this->screenSize.width*0.5, this->screenSize.height*0.25));

	auto gameOverMenu = Menu::create(restartButton);
	gameOverMenu->setPosition(Point::ZERO);
	this->popup->addChild(gameOverMenu);

	auto gameOverSprite = Sprite::createWithSpriteFrameName("dhgover");
	gameOverSprite->setPosition(Vec2(this->screenSize.width*0.5, this->screenSize.height*0.75));
	this->popup->addChild(gameOverSprite);

	auto scoreLabel = Label::createWithTTF(__String::createWithFormat("Score: %d", this->score)->getCString(), RESOURCES_FONT_COMIC_SANS, 60);
	scoreLabel->setPosition(Vec2(this->screenSize.width*0.5, this->screenSize.height*0.6));
	scoreLabel->runAction(Sequence::create(
			DelayTime::create(0.5),
			EaseSineIn::create(ScaleTo::create(0.25, 1.1)),
			EaseSineOut::create(ScaleTo::create(0.25, 1)), 
			NULL));
	this->popup->addChild(scoreLabel);

	// fetch old high score from browser's local storage
	auto oldHighScore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
	auto highScoreLabel = Label::createWithTTF(__String::createWithFormat("Your Best: %d", oldHighScore)->getCString(), RESOURCES_FONT_COMIC_SANS, 60);
	highScoreLabel->setPosition(Vec2(this->screenSize.width*0.5, this->screenSize.height*0.5));
	this->popup->addChild(highScoreLabel);

	// check if new high score has been achieved
	if (this->score > oldHighScore)
	{
		// save the new high score
		UserDefault::getInstance()->setIntegerForKey(HIGHSCORE_KEY, this->score);

		auto actionMoveDone = CallFuncN::create([&](Node *node) {
			node->removeFromParentAndCleanup(true);
		});

		// animate the button suggesting that a new high score has been achieved
		highScoreLabel->runAction(Sequence::create(
			DelayTime::create(1),
			EaseSineIn::create(ScaleTo::create(0.25, 1.1)),
			CallFuncN::create([&](Node *node) {
				highScoreLabel->setString(__String::createWithFormat("Your Best: %d", this->score)->getCString());
			}),
			EaseSineOut::create(ScaleTo::create(0.25, 1)),
			NULL));
	}
}

void GameWorld::onRestartClicked(cocos2d::Ref* ref)
{
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, GameWorld::createScene()));
}