#include "BackgroundManager.h"

bool BackgroundManager::init()
{
	if ( !Node::init() )
    {
        return false;
    }

	// create a CCDrawNode to draw the stars
	auto star = DrawNode::create();
	addChild(star);

	// draw a white circle with varying opacity & position
	for(int i = 0; i < MAX_STARS; ++i)
	{
		auto position = Vec2(CCRANDOM_0_1() * SCREEN_SIZE.width, CCRANDOM_0_1() * SCREEN_SIZE.height);
		float alpha = 0.25f + CCRANDOM_0_1() * 0.75f;
		star->drawDot(position, 1, Color4F(1, 1, 1, alpha));
	}

	return true;
}