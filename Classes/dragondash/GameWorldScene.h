#ifndef _DRAGONDASH_GAMEWORLD_SCENE_H_
#define _DRAGONDASH_GAMEWORLD_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "Resources.h"
#include "dragondash\TowerManager.h"
#include "dragondash\DragonManager.h"
#include "dragondash\FairyTaleManager.h"

USING_NS_CC;

namespace dragondash 
{
	class TowerManager;
	class DragonManager;
	class FairyTaleManager;

	class GameWorld : public cocos2d::Layer
	{
		Label* scoreLabel;
		Sprite* tutorialSprite;
		LayerColor* popup;
		
		// managers
		TowerManager* towerManager;
		DragonManager* dragonManager;
		FairyTaleManager* fairytaleManager;

		int score;
		bool mustAddScore;
		bool hasGameStarted;
		
		EventListenerTouchOneByOne *listener;

		public:
			Size screenSize;
			SpriteBatchNode* spriteBatchNode;
			int castleRoof = 0;

			static cocos2d::Scene* createScene();

			virtual bool init();

			// a selector callback
			void menuCloseCallback(cocos2d::Ref* pSender);

			// implement the "static create()" method manually
			CREATE_FUNC(GameWorld);

			void onGameOver(Node *node);
		private:
			void createHUD();
			void update(float dt);
			void checkCollisions();
			void incrementScore();
			bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
			void showGameOverPopup();
			void onRestartClicked(cocos2d::Ref* ref);
	};
}

#endif // _DRAGONDASH_GAMEWORLD_SCENE_H_
