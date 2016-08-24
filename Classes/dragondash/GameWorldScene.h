#ifndef _DRAGONDASH_GAMEWORLD_SCENE_H_
#define _DRAGONDASH_GAMEWORLD_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "Resources.h"

USING_NS_CC;

class GameWorld : public cocos2d::Layer
{
	public:
		static cocos2d::Scene* createScene();

		virtual bool init();

		// a selector callback
		void menuCloseCallback(cocos2d::Ref* pSender);

		// implement the "static create()" method manually
		CREATE_FUNC(GameWorld);

		Size screenSize;
		SpriteBatchNode* spriteBatchNode;
};

#endif // _DRAGONDASH_GAMEWORLD_SCENE_H_
