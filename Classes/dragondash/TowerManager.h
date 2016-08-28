#ifndef _DRAGONDASH_TOWERMANAGER_SCENE_H_
#define _DRAGONDASH_TOWERMANAGER_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "GameResources.h"
#include "dragondash\DragonDashDefinition.h"
#include "dragondash\GameWorldScene.h"

USING_NS_CC;

namespace dragondash 
{
	class GameWorld;
	class TowerManager : public cocos2d::Node
	{
		cocos2d::Vector<TowerManager *> towers;
		
		GameWorld* gameworld;
		Vec2 position;
		Size screenSize;
		Size towerSpriteSize;

		int firstTowerIndex;
		int lastTowerIndex;

		public:
			TowerManager(Vec2 position);
			TowerManager(GameWorld* gameworld);
			virtual bool init();
			void createTower(Vec2 position);
			void update();
			void repositionTower(int index);
			Vec2 getNextTowerPosition();
			TowerManager* getFrontTower();

			Sprite* lowerSprite;
			Sprite* upperSprite;
	};
}

#endif // _DRAGONDASH_TOWERMANAGER_SCENE_H_
