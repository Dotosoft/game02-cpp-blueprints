#ifndef _COLORSMASH_MAINMENU_SCENE_H_
#define _COLORSMASH_MAINMENU_SCENE_H_

#include <math.h>
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "colorsmasher\ColourSmashDefinition.h"
#include "colorsmasher\GameWorldScene.h"

USING_NS_CC;

namespace colorsmasher 
{
	class MainMenu : public cocos2d::Layer
	{
		public:
			static cocos2d::Scene* createScene();

			virtual bool init();

			// a selector callback
			void menuCloseCallback(cocos2d::Ref* pSender);
			void onPlayClicked(cocos2d::Ref* pSender);
			void doAnimation(float dt);
			Vec2 getRandomPositionForTile();
			Color3B getColourForTile(E_COLOUR_TYPE colourData);

			// implement the "static create()" method manually
			CREATE_FUNC(MainMenu);

			float TILE_SIZE;
			Vec2 GAMEPLAY_OFFSET;
			float tileZoomFactor;
	};
}

#endif // _COLORSMASH_MAINMENU_SCENE_H_
