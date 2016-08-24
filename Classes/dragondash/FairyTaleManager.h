#ifndef _DRAGONDASH_FAIRYTALEMANAGER_SCENE_H_
#define _DRAGONDASH_FAIRYTALEMANAGER_SCENE_H_

#include <math.h>

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "Resources.h"
#include "dragondash\DragonDashDefinition.h"
#include "dragondash\GameWorldScene.h"

USING_NS_CC;

namespace dragondash 
{
	class FairyTaleManager : public cocos2d::Node
	{
		// save reference to GameWorld
		ParentScene* parent;
		Size screenSize;
	
		// initialise variables
		Size castleSpriteSize;
		cocos2d::Vector<Sprite *> castleSprites;
		int lastCastleIndex = 0;

		Size silhouetteSpriteSize;
		cocos2d::Vector<Sprite *> silhouetteSprites;
		int lastSilhouetteIndex = 0;

		public:
			FairyTaleManager(ParentScene* parent);
			virtual bool init();
			void update();
		private:
			void createCastle();
			void createSilhouette();
			void createStars();
			void updateCastle();
			void updateSilhouette();
	};
}

#endif // _DRAGONDASH_FAIRYTALEMANAGER_SCENE_H_
