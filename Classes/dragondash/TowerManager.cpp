#include "dragondash\TowerManager.h"

using namespace dragondash;

dragondash::TowerManager::TowerManager(Vec2 position)
{
	this->lowerSprite = NULL;
	this->upperSprite = NULL;
	this->position = position;
}

dragondash::TowerManager::TowerManager(GameWorld * parent)
{
	// save reference to GameWorld
	this->gameworld = parent;
	this->screenSize = parent->screenSize;
	// initialise variables
	// this->towers = [];
	this->towerSpriteSize = Size::ZERO;
	this->firstTowerIndex = 0;
	this->lastTowerIndex = 0;
}

bool dragondash::TowerManager::init()
{
	// record size of the tower's sprite
	this->towerSpriteSize = SpriteFrameCache::getInstance()->getSpriteFrameByName("opst_02")->getOriginalSize() * this->gameworld->scaleFactor;

	// create the first pair of towers
	// they should be two whole screens away from the dragon
	auto initialPosition = Vec2(this->screenSize.width * 2, this->screenSize.height*0.5);
	this->firstTowerIndex = 0;
	this->createTower(initialPosition);
	// create the remaining towers
	this->lastTowerIndex = 0;
	this->createTower(this->getNextTowerPosition());
	this->lastTowerIndex = 1;
	this->createTower(this->getNextTowerPosition());
	this->lastTowerIndex = 2;

	return true;
}

void dragondash::TowerManager::createTower(Vec2 position)
{
	// create a new tower and add it to the array
	auto tower = new TowerManager(position);
	this->towers.pushBack(tower);

	// create lower tower sprite & add it to GameWorld's batch node
	tower->lowerSprite = Sprite::createWithSpriteFrameName("opst_02");
	tower->lowerSprite->setScale(this->gameworld->scaleFactor);
	tower->lowerSprite->setPositionX(position.x);
	tower->lowerSprite->setPositionY(position.y + VERT_GAP_BWN_TOWERS * -0.5 + this->towerSpriteSize.height * -0.5);
	this->gameworld->spriteBatchNode->addChild(tower->lowerSprite, E_ZORDER::E_LAYER_TOWER);

	// create upper tower sprite & add it to GameWorld's batch node
	tower->upperSprite = Sprite::createWithSpriteFrameName("opst_01");
	tower->upperSprite->setScale(this->gameworld->scaleFactor);
	tower->upperSprite->setPositionX(position.x);
	tower->upperSprite->setPositionY(position.y + VERT_GAP_BWN_TOWERS * 0.5 + this->towerSpriteSize.height * 0.5);
	this->gameworld->spriteBatchNode->addChild(tower->upperSprite, E_ZORDER::E_LAYER_TOWER);
}

void dragondash::TowerManager::update()
{
	TowerManager* tower;
	for (int i = 0; i < this->towers.size(); ++i)
	{
		tower = this->towers.at(i);
		// first update the position of the tower
		tower->position.x -= MAX_SCROLLING_SPEED;
		tower->lowerSprite->setPosition(tower->position.x, tower->lowerSprite->getPositionY());
		tower->upperSprite->setPosition(tower->position.x, tower->upperSprite->getPositionY());

		// if the tower has moved out of the screen, reposition them at the end
		if (tower->position.x < this->towerSpriteSize.width * -0.5)
		{
			this->repositionTower(i);
			// this tower now becomes the tower at the end
			this->lastTowerIndex = i;
			// that means some other tower has become first
			this->firstTowerIndex = ((i + 1) >= this->towers.size()) ? 0 : (i + 1);
		}
	}
}

void dragondash::TowerManager::repositionTower(int index)
{
	auto tower = this->towers.at(index);
	// update tower's position and sprites
	tower->position = this->getNextTowerPosition();
	tower->lowerSprite->setPosition(tower->position.x, tower->position.y + VERT_GAP_BWN_TOWERS * -0.5 + this->towerSpriteSize.height * -0.5);
	tower->upperSprite->setPosition(tower->position.x, tower->position.y + VERT_GAP_BWN_TOWERS * 0.5 + this->towerSpriteSize.height * 0.5);
}

Vec2 dragondash::TowerManager::getNextTowerPosition()
{
	// randomly select either above or below last tower
	bool isAbove = (CCRANDOM_0_1() > 0.5);
	float offset = CCRANDOM_0_1() * VERT_GAP_BWN_TOWERS * 0.75;
	offset *= (isAbove) ? 1 : -1;

	// new position calculated by adding to last tower's position
	float newPositionX = this->towers.at(this->lastTowerIndex)->position.x + this->screenSize.width * 0.5f;
	float newPositionY = this->towers.at(this->lastTowerIndex)->position.y + offset;

	// limit the point to stay within 30-80% of the screen
	if (newPositionY >= this->screenSize.height * 0.8) {
		newPositionY -= VERT_GAP_BWN_TOWERS;
	} else if (newPositionY <= this->screenSize.height * 0.3) {
		newPositionY += VERT_GAP_BWN_TOWERS;
	}

	// return the new tower position
	return Vec2(newPositionX, newPositionY);
}

dragondash::TowerManager* dragondash::TowerManager::getFrontTower()
{
	return this->towers.at(this->firstTowerIndex);
}
