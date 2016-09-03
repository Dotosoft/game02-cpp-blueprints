#include "spacecraze\Brick.h"

using namespace spacecraze;

Brick::Brick(const char* frame_name)
{
	score_ = 0;

	// different size for different bricks
	if(strstr(frame_name, "1"))
	{
		size_ = Size(50, 20);
	}
	else if(strstr(frame_name, "2"))
	{
		size_ = Size(55, 20);
	}
}

Brick::~Brick()
{}

Brick* Brick::createWithSpriteFrameName(const char* frame_name)
{
	auto brick = new Brick(frame_name);
	if(brick && brick->initWithSpriteFrameName(frame_name))
	{
		brick->autorelease();
		return brick;
	}
	CC_SAFE_DELETE(brick);
	return NULL;
}

int Brick::Crumble()
{
	// play an animation when this brick is hit by player bullet
	auto blast = ScaleTo::create(0.25f, 0.0f);
	auto remove = RemoveSelf::create(true);
	runAction(Sequence::createWithTwoActions(blast, remove));

	SOUND_ENGINE->playEffect(RESOURCES_SPACECRAZE_SOUND_BLAST_BRICK);

	// return score_ so it can be credited to the player
	return score_;
}
