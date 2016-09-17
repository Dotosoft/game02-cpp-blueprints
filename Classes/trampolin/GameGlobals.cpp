#include "GameGlobals.h"

Size GameGlobals::screen_size_ = Size::ZERO;

void GameGlobals::Init()
{
	screen_size_ = Director::getInstance()->getWinSize();
	LoadData();
}

void GameGlobals::LoadData()
{
	// add Resources folder to search path. This is necessary when releasing for win32
	FileUtils::getInstance()->addSearchPath("Resources");

	// load sprite sheet/s
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("cjtexset_01.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("cjtexset_02.plist");

	// load sound effects & background music
	SOUND_ENGINE->preloadEffect("base_platform.wav");
	SOUND_ENGINE->preloadEffect("bottle_rocket.wav");
	SOUND_ENGINE->preloadEffect("collectible.wav");
	SOUND_ENGINE->preloadEffect("game_over.wav");
	SOUND_ENGINE->preloadEffect("platform.wav");
	
	// create and add platform animation
	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.05f);
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("cjump04.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("cjump03.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("cjump02.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("cjump01.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("cjump05.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("cjump01.png"));
	AnimationCache::getInstance()->addAnimation(animation, "platform_animation");
}

// function takes comma separated string & returns vector of values
vector<float> GameGlobals::GetFloatListFromString(string input)
{
	vector<float> result;
	result.clear();

	if(input == "")
		return result;

	stringstream ss(input);
	float i;
	while (ss >> i)
	{
		result.push_back(i);
		if (ss.peek() == ',')
			ss.ignore();
	}
	return result;
}

// function takes comma separated string & returns CCPoint
Point GameGlobals::GetPointFromString(string input)
{
	Point point = Point::ZERO;
	if(input == "")
		return point;
	vector<float> list = GetFloatListFromString(input);
	point.x = list[0];
	point.y = list[1];
	return point;
}
