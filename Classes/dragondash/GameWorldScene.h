#ifndef _DRAGONDASH_GAMEWORLD_SCENE_H_
#define _DRAGONDASH_GAMEWORLD_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "Resources.h"
#include "dragondash\ParentScene.h"
#include "dragondash\TowerManager.h"
#include "dragondash\DragonManager.h"
#include "dragondash\FairyTaleManager.h"

USING_NS_CC;

namespace dragondash 
{
	class GameWorld : public ParentScene
	{
		int score;
		Label* scoreLabel;
		bool mustAddScore;
		Sprite* tutorialSprite;
		LayerColor* popup;
		bool hasGameStarted;
		// managers
		TowerManager* towerManager;
		DragonManager* dragonManager;
		FairyTaleManager* fairytaleManager;

		public:
			static cocos2d::Scene* createScene();

			virtual bool init();

			// a selector callback
			void menuCloseCallback(cocos2d::Ref* pSender);

			// implement the "static create()" method manually
			CREATE_FUNC(GameWorld);
		private:
			void createHUD();
			void update(float dt);
			void checkCollisions();
			void incrementScore();
			void onGameOver();
			bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
			void showGameOverPopup();
			void onRestartClicked();
	};
}

#endif // _DRAGONDASH_GAMEWORLD_SCENE_H_
