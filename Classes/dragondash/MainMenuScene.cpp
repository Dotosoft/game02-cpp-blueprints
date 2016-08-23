#include "dragondash\MainMenuScene.h"

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

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(RESOURCES_PLIST_SHEET_DRAGON);

	this->screenSize = Director::getInstance()->getWinSize();
	this->spriteBatchNode = SpriteBatchNode::create(RESOURCES_IMAGE_SHEET_DRAGON, 256);
	this->addChild(this->spriteBatchNode, E_ZORDER::E_LAYER_BG + 1);
    
	this->addDragonAnimation();
	this->addDragon();
    
    return true;
}

void MainMenu::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
	#endif
}

void MainMenu::addDragonAnimation()
{
	cocos2d::Vector<SpriteFrame *> spriteFrames;
	spriteFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("dhch_1"));
	spriteFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("dhch_2"));
	spriteFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("dhch_3"));
	spriteFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("dhch_2"));
	spriteFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("dhch_1"));

	// create the animation with the array of sprite frames and delay per frame
	auto dragonAnimation = Animation::createWithSpriteFrames(spriteFrames, 0.1f);
	// add the created animation to the cache with a name so it can be reused
	AnimationCache::getInstance()->addAnimation(dragonAnimation, "dragonFlying");
}

void MainMenu::addDragon()
{
	// create sprite and add to the sprite batch node
	auto dragonSprite = Sprite::createWithSpriteFrameName("dhch_1");
	dragonSprite->setPosition(Vec2(this->screenSize.width * 0.2, this->screenSize.height * 0.5));
	this->spriteBatchNode->addChild(dragonSprite, E_ZORDER::E_LAYER_PLAYER);

	// fetch flying animation from cache & repeat it on the dragon's  sprite
	auto animation = AnimationCache::getInstance()->getAnimation("dragonFlying");
	dragonSprite->runAction(RepeatForever::create(Animate::create(animation)));

	// create a hover movement and repeat it on the dragon's sprite
	auto flySequence = Sequence::create(
		EaseSineOut::create(MoveBy::create(animation->getDuration() / 2, Vec2(0, 10))), 
		EaseSineOut::create(MoveBy::create(animation->getDuration() / 2, Vec2(0, -10))),
		NULL);
	dragonSprite->runAction(RepeatForever::create(flySequence));
}