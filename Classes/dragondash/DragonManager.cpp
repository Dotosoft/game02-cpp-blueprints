#include "dragondash\DragonManager.h"

using namespace dragondash;

DragonManager::DragonManager(GameWorld* parent)
{
	// save reference to GameWorld
	this->gameworld = parent;
	this->screenSize = parent->screenSize;
	// initialise variables
	this->dragonSprite = NULL;
	this->dragonSpeed = Point::ZERO;
	this->dragonPosition = Vec2::ZERO;
	this->mustApplyGravity = false;
}

bool DragonManager::init()
{
	// create sprite and add to GameWorld's sprite batch node
	this->dragonSprite = Sprite::createWithSpriteFrameName("dhch_1");
	this->dragonPosition = Vec2(this->screenSize.width * 0.2, this->screenSize.height * 0.5);
	this->dragonSprite->setPosition(this->dragonPosition);
	this->gameworld->spriteBatchNode->addChild(this->dragonSprite, E_ZORDER::E_LAYER_PLAYER);

	// fetch flying animation from cache & repeat it on the dragon's  sprite
	auto animation = AnimationCache::getInstance()->getAnimation("dragonFlying");
	auto repeatedAnimation = RepeatForever::create(Animate::create(animation));
	repeatedAnimation->setTag(ANIMATION_ACTION_TAG);
	this->dragonSprite->runAction(repeatedAnimation);

	// create a hover movement and repeat it on the dragon's sprite
	auto flySequence = Sequence::create(
		EaseSineOut::create(MoveBy::create(animation->getDuration() / 2, Vec2(0, 10))), 
		EaseSineOut::create(MoveBy::create(animation->getDuration() / 2, Vec2(0, -10))),
		NULL);
	auto repeatedFlySequence = RepeatForever::create(flySequence);
	repeatedFlySequence->setTag(MOVEMENT_ACTION_TAG);
	this->dragonSprite->runAction(repeatedFlySequence);

	return true;
}

void dragondash::DragonManager::onGameStart()
{
	// hover should stop once the game has started
	this->dragonSprite->stopActionByTag(MOVEMENT_ACTION_TAG);
	// gravity should be applied once the game has started
	this->mustApplyGravity = true;
}

void dragondash::DragonManager::update(float dt)
{
	// calculate bounding box after applying gravity
	auto newAABB = this->dragonSprite->getBoundingBox();
	newAABB.origin.y = newAABB.origin.y + this->dragonSpeed.y;
	// newAABB->setY(newAABB->getY() + this->dragonSpeed.y);

	// check if the dragon has touched the roof of the castle
	if(newAABB.origin.y <= this->gameworld->castleRoof)
	{
		// stop downward movement and set position to the roof of the castle
		this->dragonSpeed.y = 0;
		this->dragonPosition.y = this->gameworld->castleRoof + newAABB.size.height * 0.5f;

		// dragon must die
		this->dragonDeath();
		// stop the update loop
		this->gameworld->unscheduleUpdate();
	}
	// apply gravity only if game has started
	else if (this->mustApplyGravity)
	{
		// clamp gravity to a maximum of MAX_DRAGON_SPEED & add it
		this->dragonSpeed.y = ((this->dragonSpeed.y + GRAVITY) < MAX_DRAGON_SPEED) ? MAX_DRAGON_SPEED : (this->dragonSpeed.y + GRAVITY);
	}

	// update position
	this->dragonPosition.y += this->dragonSpeed.y;
	this->dragonSprite->setPosition(this->dragonPosition);
}

void dragondash::DragonManager::dragonFlap()
{
	// don't flap if dragon will leave the top of the screen
	if (this->dragonPosition.y + FLAP_FORCE >= this->screenSize.height)
		return;

	// add flap force to speed
	this->dragonSpeed.y = FLAP_FORCE;

	SimpleAudioEngine::getInstance()->playEffect(RESOURCES_SFX_FLAP);
}

void dragondash::DragonManager::dragonDeath()
{
	// fall miserably to the roof of the castle
	auto rise = EaseSineOut::create(MoveBy::create(0.25, Vec2(0, this->dragonSprite->getContentSize().height)));
	auto fall = EaseSineIn::create(MoveTo::create(0.5, Vec2(this->screenSize.width * 0.2, this->gameworld->castleRoof)));
	// inform GameWorld that dragon is no more :(
	auto finish = CallFuncN::create(this->gameworld, callfuncN_selector(GameWorld::onGameOver));
	// stop the frame based animation...dragon can't fly once its dead
	this->dragonSprite->stopAllActions();
	this->dragonSprite->runAction(Sequence::create(rise, fall, finish, NULL));

	SimpleAudioEngine::getInstance()->playEffect(RESOURCES_SFX_CRASH);
}