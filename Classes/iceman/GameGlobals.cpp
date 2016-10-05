#include "GameGlobals.h"

Size GameGlobals::screen_size_ = Size::ZERO;
int GameGlobals::level_number_ = 0;
int GameGlobals::hero_lives_left_ = 0;

void GameGlobals::Init()
{
	screen_size_ = Director::getInstance()->getWinSize();
	LoadData();
}

void GameGlobals::LoadData()
{
	// add Resources folder to search path. This is necessary when releasing for win32
	FileUtils::getInstance()->addSearchPath("Resources");

	// load sound effects & background music
	SOUND_ENGINE->preloadEffect("brick.wav");
	SOUND_ENGINE->preloadEffect("enemy_death.wav");
	SOUND_ENGINE->preloadEffect("game_over.wav");
	SOUND_ENGINE->preloadEffect("hero_death.wav");
	SOUND_ENGINE->preloadEffect("jump.wav");
	SOUND_ENGINE->preloadEffect("level_complete.wav");
	SOUND_ENGINE->preloadEffect("swing.wav");

	// create and add animations
	LoadAnimations();
}

void GameGlobals::LoadAnimations()
{
	Animation* animation = NULL;

	// Hero animations
	animation = Animation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(-1);
	animation->addSpriteFrame(Sprite::create("idle0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("idle0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "HeroIdle");
	
	animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	animation->setLoops(-1);
	animation->addSpriteFrame(Sprite::create("walking0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("walking0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "HeroWalking");
	
	animation = Animation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(-1);
	animation->addSpriteFrame(Sprite::create("swinging0002.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("swinging0001.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "HeroSwinging");
	
	animation = Animation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(2);
	animation->addSpriteFrame(Sprite::create("dying0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("dying0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "HeroDying");
	
	animation = Animation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(3);
	animation->addSpriteFrame(Sprite::create("win0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("win0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "HeroWinning");

	// Enemy animations	
	animation = Animation::create();
	animation->setDelayPerUnit(0.2f);
	animation->setLoops(-1);
	animation->addSpriteFrame(Sprite::create("enemy walking0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("enemy walking0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "EnemyWalking");
	
	animation = Animation::create();
	animation->setDelayPerUnit(0.25f);
	animation->addSpriteFrame(Sprite::create("enemyBuilding0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("enemyBuilding0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "EnemyBuilding");
	
	animation = Animation::create();
	animation->setDelayPerUnit(0.25f);
	animation->addSpriteFrame(Sprite::create("enemy dying0001.png")->getSpriteFrame());
	animation->addSpriteFrame(Sprite::create("enemy dying0002.png")->getSpriteFrame());
	AnimationCache::getInstance()->addAnimation(animation, "EnemyDying");
}