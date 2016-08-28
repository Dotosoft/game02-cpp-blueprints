#ifndef _DRAGONDASH_DRAGONMANAGER_SCENE_H_
#define _DRAGONDASH_DRAGONMANAGER_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "GameResources.h"
#include "dragondash\DragonDashDefinition.h"
#include "dragondash\GameWorldScene.h"

USING_NS_CC;
using namespace CocosDenshion;

namespace dragondash 
{
	class GameWorld;
	class DragonManager : public cocos2d::Node
	{
		GameWorld* gameworld;
		Size screenSize;
		Point dragonSpeed;
		Vec2 dragonPosition;
		bool mustApplyGravity;

		public:
			DragonManager(GameWorld* parent);
			virtual bool init();
			void onGameStart();
			void update(float dt);
			void dragonFlap();
			void dragonDeath();

			Sprite* dragonSprite;
	};
}

#endif // _DRAGONDASH_DRAGONMANAGER_SCENE_H_
