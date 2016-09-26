#include "GameGlobals.h"

Size GameGlobals::screen_size_ = Size::ZERO;

void GameGlobals::Init()
{
	srand(time(0));
	screen_size_ = Director::getInstance()->getWinSize();
	LoadData();
}

void GameGlobals::LoadData()
{
	// add Resources folder to search path. This is necessary when releasing for win32
	FileUtils::getInstance()->addSearchPath("Resources");
}

Color4F GameGlobals::GetRandomColor()
{
	return Color4F(0.3f + CCRANDOM_0_1() * 0.7f, 0.3f + CCRANDOM_0_1() * 0.7f, 0.3f + CCRANDOM_0_1() * 0.7f, 1.0f);
}