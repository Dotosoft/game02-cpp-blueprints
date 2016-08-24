#include "dragondash\FairyTaleManager.h"
using namespace dragondash;

FairyTaleManager::FairyTaleManager(ParentScene* parent)
{
	// save reference to GameWorld
	this->parent = parent;
	this->screenSize = Director::getInstance()->getWinSize();

	// initialise variables
	this->castleSpriteSize = Size::ZERO;
	this->lastCastleIndex = 0;
	this->silhouetteSpriteSize = Size::ZERO;
	this->lastSilhouetteIndex = 0;
}

bool FairyTaleManager::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Node::init())
	{
		return false;
	}

	// this makes a nice midnight sky
	auto background = LayerGradient::create(Color4B(15, 15, 25, 255), Color4B(84, 83, 104, 255));
	this->parent->addChild(background, E_ZORDER::E_LAYER_BG);

	this->createCastle();
	this->createSilhouette();
	this->createStars();

	return true;
}

void FairyTaleManager::createCastle()
{
	// record size of the castle wall sprite
	this->castleSpriteSize = SpriteFrameCache::getInstance()->getSpriteFrameByName("dhbase")->getOriginalSize();
	// initial position
	auto nextPosition = this->castleSpriteSize.width * 0.5f;
	// fill up one & a half screen
	while (nextPosition < this->screenSize.width * 1.5f)
	{
		// create castle wall sprite and add it to the parent's batch node
		Sprite* castleSprite = Sprite::createWithSpriteFrameName("dhbase");
		// castleSprite->setPosition( Vec2(nextPosition, CASTLE_SPRITE_Y) );
		castleSprite->setPosition( Vec2(nextPosition, -50) );
		this->parent->spriteBatchNode->addChild(castleSprite, E_ZORDER::E_LAYER_CASTLE);
		// store this sprite...we need to update it
		this->castleSprites.pushBack(castleSprite);
		// the next wall depends on this variable
		nextPosition += this->castleSpriteSize.width;
	}
	// we need this to position the next wall sprite
	this->lastCastleIndex = this->castleSprites.size() - 1;
}

void FairyTaleManager::createSilhouette()
{
	// record size of the silhouette sprite
	this->silhouetteSpriteSize = SpriteFrameCache::getInstance()->getSpriteFrameByName("dhbush")->getOriginalSize();
	// initial position
	auto nextPosition = 0;
	// fill up one & a half screen
	while (nextPosition < this->screenSize.width * 1.5)
	{
		// create silhouette sprite and add it to the parent's batch node
		auto silhouetteSprite = Sprite::createWithSpriteFrameName("dhbush");
		// silhouetteSprite->setPosition(nextPosition, SILHOUETTE_SPRITE_Y);
		silhouetteSprite->setPosition(nextPosition, 100);
		this->parent->spriteBatchNode->addChild(silhouetteSprite, E_ZORDER::E_LAYER_SILHOUETTE);
		// store this sprite...we need to update it
		this->silhouetteSprites.pushBack(silhouetteSprite);
		// the next silhouette depends on this variable
		nextPosition += this->silhouetteSpriteSize.width;
	}
	// we need this to position the next silhouette sprite
	this->lastSilhouetteIndex = this->silhouetteSprites.size() - 1;
}

void FairyTaleManager::createStars()
{
	// random number of stars...this night sky always changes
	// auto numStars = int(MAX_STARS) + floor(CCRANDOM_0_1() * int(MAX_STARS)));
	auto numStars = 15 + floor(CCRANDOM_0_1() * 15);
	for (int i = 0; i < numStars; ++i)
	{
		Sprite* star;
		// either big star or small
		if (random() > 0.5)
			star = Sprite::createWithSpriteFrameName("dhstar1");
		else
			star = Sprite::createWithSpriteFrameName("dhstar2");

		// random position
		auto position = Vec2(CCRANDOM_0_1() * this->screenSize.width, CCRANDOM_0_1() * this->screenSize.height);
		star->setPosition(position);
		// twinkle twinkle randomly star
		float duration = 1 + CCRANDOM_0_1() * 2;
		auto action = RepeatForever::create(
			Sequence::create(DelayTime::create(duration * 2), FadeOut::create(duration), FadeIn::create(duration), NULL)
		);
		star->runAction(action);
		// add this too the batch node as well
		this->parent->spriteBatchNode->addChild(star);
	}
}

void FairyTaleManager::update()
{
	this->updateCastle();
	this->updateSilhouette();
}

void FairyTaleManager::updateCastle()
{
	for (int i = 0; i < this->castleSprites.size(); ++i)
	{
		// first update the position based on the scroll speed
		auto castleSprite = this->castleSprites.at(i);
		// castleSprite.setPosition(castleSprite.getPositionX() - MAX_SCROLLING_SPEED, castleSprite.getPositionY());
		castleSprite->setPosition(castleSprite->getPositionX() - 6, castleSprite->getPositionY());

		// check if the sprite has gone completely out of the left edge of the screen
		if (castleSprite->getPositionX() < (this->castleSpriteSize.width * -0.5))
		{
			// reposition it after the last wall sprite
			// auto positionX = this->castleSprites.at(this->lastCastleIndex)->getPositionX() + this->castleSpriteSize.width - MAX_SCROLLING_SPEED;
			auto positionX = this->castleSprites.at(this->lastCastleIndex)->getPositionX() + this->castleSpriteSize.width - 6;
			castleSprite->setPosition(positionX, castleSprite->getPositionY());
			// this sprite now becomes the new last wall
			this->lastCastleIndex = i;
		}
	}
}

void FairyTaleManager::updateSilhouette()
{
	for (int i = 0; i < this->silhouetteSprites.size(); ++i)
	{
		// first update the position based on the scroll speed
		auto silhouetteSprite = this->silhouetteSprites.at(i);
		// silhouetteSprite->setPosition(this->silhouetteSprites.at(i).getPositionX() - MAX_SCROLLING_SPEED * 0.3f, silhouetteSprite->getPositionY());
		silhouetteSprite->setPosition(this->silhouetteSprites.at(i)->getPositionX() - 6 * 0.3f, silhouetteSprite->getPositionY());

		// check if the sprite has gone completely out of the left edge of the screen
		if (silhouetteSprite->getPositionX() < (this->silhouetteSpriteSize.width * -0.5))
		{
			// reposition it after the last silhouette sprite
			// auto positionX = this->silhouetteSprites.at(this->lastSilhouetteIndex).getPositionX() + this->silhouetteSpriteSize.width - MAX_SCROLLING_SPEED*0.3;
			auto positionX = this->silhouetteSprites.at(this->lastSilhouetteIndex)->getPositionX() + this->silhouetteSpriteSize.width - 6 * 0.3f;
			silhouetteSprite->setPosition(positionX, silhouetteSprite->getPositionY());
			// this sprite now becomes the new last silhouette
			this->lastSilhouetteIndex = i;
		}
	}
}