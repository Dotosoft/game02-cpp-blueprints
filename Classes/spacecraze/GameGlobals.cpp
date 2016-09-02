#include "spacecraze\GameGlobals.h"

using namespace spacecraze;

Size GameGlobals::screen_size_ = Size::ZERO;

void GameGlobals::Init()
{
	screen_size_ = CCDirector::sharedDirector()->getWinSize();
	LoadData();
}

void GameGlobals::LoadData()
{
	// add Resources folder to search path. This is necessary when releasing for win32
	CCFileUtils::sharedFileUtils()->addSearchPath("Resources");

	// load sprite sheet/s
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("spacetex.plist");

	// load sound effects & background music
	SOUND_ENGINE->preloadEffect("blast_brick.wav");
	SOUND_ENGINE->preloadEffect("blast_enemy.wav");
	SOUND_ENGINE->preloadEffect("blast_player.wav");
	SOUND_ENGINE->preloadEffect("game_over.wav");
	SOUND_ENGINE->preloadEffect("level_complete.wav");
	SOUND_ENGINE->preloadEffect("shoot_enemy.wav");
	SOUND_ENGINE->preloadEffect("shoot_player.wav");
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
	auto point = Point::ZERO;
	if(input == "") return point;
	vector<float> list = GetFloatListFromString(input);
	point.x = list[0];
	point.y = list[1];
	return point;
}