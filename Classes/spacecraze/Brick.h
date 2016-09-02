#ifndef _SPACECRAZE_BRICK_H_
#define _SPACECRAZE_BRICK_H_

#include "spacecraze\GameGlobals.h"
#include "spacecraze\CustomSprite.h"
#include "GameResources.h"

namespace spacecraze
{
	class Brick : public CustomSprite
	{
		public:
			Brick(const char* frame_name);
			virtual ~Brick();

			// returns an autorelease Brick
			static Brick* createWithSpriteFrameName(const char* frame_name);

			// function called when this brick is hit by player bullet
			int Crumble();

			CC_SYNTHESIZE(int, score_, Score);
	};
}

#endif // _SPACECRAZE_BRICK_H_
