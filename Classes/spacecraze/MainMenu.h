#ifndef _SPACECRAZE_MAINMENU_H_
#define _SPACECRAZE_MAINMENU_H_

#include "spacecraze\GameGlobals.h"

namespace spacecraze
{
	class MainMenu : public CCLayer
	{
	public:
		// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
		virtual bool init();

		// there's no 'id' in cpp, so we recommend returning the class instance pointer
		static CCScene* scene();

		// implement the "static node()" method manually
		CREATE_FUNC(MainMenu);

		// create some chaos
		void Animate(float dt);
		// play button handler
		void OnPlayClicked(CCObject* sender);

		CCArray* stars_;
		CCSpriteBatchNode* sprite_batch_node_;
	};
}

#endif // _SPACECRAZE_MAINMENU_H_
