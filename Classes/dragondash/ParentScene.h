#ifndef _DRAGONDASH_PARENT_SCENE_H_
#define _DRAGONDASH_PARENT_SCENE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "Resources.h"

USING_NS_CC;

namespace dragondash 
{
	class ParentScene : public cocos2d::Layer
	{
		public:
			Size screenSize;
			SpriteBatchNode* spriteBatchNode;
			int castleRoof = 0;
	};
}

#endif // _DRAGONDASH_PARENT_SCENE_H_
