#ifndef _DRAGONDASH_HELLOWORLD_SCENE_H_
#define _DRAGONDASH_HELLOWORLD_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "..\Resources.h"

USING_NS_CC;

class MainMenu : public cocos2d::Layer
{
	public:
		static cocos2d::Scene* createScene();

		virtual bool init();

		// a selector callback
		void menuCloseCallback(cocos2d::Ref* pSender);

		// implement the "static create()" method manually
		CREATE_FUNC(MainMenu);
};

#endif // _DRAGONDASH_HELLOWORLD_SCENE_H_
