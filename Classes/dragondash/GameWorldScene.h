#ifndef _DRAGONDASH_GAMEWORLD_SCENE_H_
#define _DRAGONDASH_GAMEWORLD_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "Resources.h"
#include "dragondash\ParentScene.h"

USING_NS_CC;

namespace dragondash 
{
	class GameWorld : public ParentScene
	{
		public:
			static cocos2d::Scene* createScene();

			virtual bool init();

			// a selector callback
			void menuCloseCallback(cocos2d::Ref* pSender);

			// implement the "static create()" method manually
			CREATE_FUNC(GameWorld);
	};
	}

#endif // _DRAGONDASH_GAMEWORLD_SCENE_H_
