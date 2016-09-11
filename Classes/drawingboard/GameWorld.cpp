#include "GameWorld.h"
#include "BackgroundManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Shield.h"
#include "Bomb.h"
#include "Blast.h"
#include "MissileLauncher.h"
#include "Missile.h"
#include "Popups.h"

GameWorld::GameWorld()
{
	background_ = NULL;
	boundary_rect_ = Rect::ZERO;
	player_ = NULL;
	enemies_ = NULL;
	powerups_ = NULL;
	blasts_ = NULL;
	missiles_ = NULL;
	score_label_ = NULL;
	seconds_ = 0;
	enemies_killed_total_ = 0;
	enemies_killed_combo_ = 0;
	combo_timer_ = 0;
	score_ = 0;
	is_popup_active_ = false;
}

GameWorld::~GameWorld()
{
	CC_SAFE_RELEASE_NULL(enemies_);
}

Scene* GameWorld::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	// enable accelerometer
    // setAccelerometerEnabled(true);
	Device::setAccelerometerEnabled(true);
	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameWorld::onAcceleration, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// enable touch
    // setTouchEnabled(true);
	auto listener1 = EventListenerTouchAllAtOnce::create();
	listener1->onTouchesBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
	listener1->onTouchesMoved = CC_CALLBACK_2(GameWorld::onTouchMoving, this);
	listener1->onTouchesEnded = CC_CALLBACK_2(GameWorld::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);

	CreateGame();

    return true;
}

void GameWorld::CreateGame()
{
	// initialise counters & flags
	seconds_ = 0;
	enemies_killed_total_ = 0;
	enemies_killed_combo_ = 0;
	combo_timer_ = 0;
	score_ = 0;
	is_popup_active_ = false;

	// add the stars
	background_ = BackgroundManager::create();
	addChild(background_, E_LAYER_BACKGROUND);

	CreateBoundary();
	CreatePlayer();
	CreateContainers();
	CreateHUD();

	// initially add some enemies & a powerup
	AddEnemyFormation();
	AddPowerUp();

	// schedule the update and the tick
	scheduleUpdate();
	schedule(schedule_selector(GameWorld::Tick), 1.0f);
}

void GameWorld::CreateBoundary()
{
	float offset = 0.025f;
	// calculate the position & dimension of the game's boundary
	boundary_rect_.origin.x = SCREEN_SIZE.width * offset;
	boundary_rect_.origin.y = SCREEN_SIZE.height * offset;
	boundary_rect_.size.width = SCREEN_SIZE.width - SCREEN_SIZE.width * offset * 2;
	boundary_rect_.size.height = SCREEN_SIZE.height - SCREEN_SIZE.height * offset * 4;

	// generate vertices for the boundary
	Point vertices[4] = {Point(boundary_rect_.origin.x, boundary_rect_.origin.y), 
		Point(boundary_rect_.origin.x, boundary_rect_.origin.y + boundary_rect_.size.height), 
		Point(boundary_rect_.origin.x + boundary_rect_.size.width, boundary_rect_.origin.y + boundary_rect_.size.height), 
		Point(boundary_rect_.origin.x + boundary_rect_.size.width, boundary_rect_.origin.y)};

	// draw the boundary
	DrawNode* boundary = DrawNode::create();
	boundary->drawPolygon(vertices, 4, Color4F(0, 0, 0, 0), 1, Color4F(1, 1, 1, 0.3f));
	addChild(boundary, E_LAYER_FOREGROUND);
}

void GameWorld::CreatePlayer()
{
	// create & add the player at the center of the screen
	player_ = Player::create();
	player_->game_world_ = this;
	player_->setPosition(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.5f);
	addChild(player_, E_LAYER_PLAYER);
}

void GameWorld::CreateContainers()
{
	// create & retain CCArray container lists
	enemies_ = __Array::createWithCapacity(MAX_ENEMIES);
	enemies_->retain();
	powerups_ = __Array::createWithCapacity(MAX_POWERUPS);
	powerups_->retain();
	blasts_ = __Array::createWithCapacity(MAX_BLASTS);
	blasts_->retain();
	missiles_ = __Array::createWithCapacity(MAX_MISSILES);
	missiles_->retain();
}

void GameWorld::CreateHUD()
{
	// create & add the score label
	score_label_ = Label::createWithBMFont("infont.fnt", "Score: 0");
	score_label_->setAnchorPoint(Point::ZERO);
	score_label_->setPosition(Point(SCREEN_SIZE.width * 0.1f, boundary_rect_.getMaxY() + boundary_rect_.getMinY()));
	addChild(score_label_, E_LAYER_HUD);
}

void GameWorld::AddEnemyFormation()
{
	// fetch an enemy formation formation
	EEnemyFormation type = GetEnemyFormationType();
	// fetch a list of positions for the given formation
	vector<Point> formation = GameGlobals::GetEnemyFormation(type, boundary_rect_, player_->getPosition());
	int num_enemies_to_create = formation.size();	
	int num_enemies_on_screen = enemies_->count();
	// limit the total number of enemies to MAX_ENEMIES
	if(num_enemies_on_screen + num_enemies_to_create >= MAX_ENEMIES)
	{
		num_enemies_to_create = MAX_ENEMIES - num_enemies_on_screen;
	}
	// create, add & position enemies based on the formation
	for(int i = 0; i < num_enemies_to_create; ++i)
	{
		Enemy* enemy = Enemy::create(this);
		enemy->setPosition(formation[i]);
		enemy->Spawn(i * ENEMY_SPAWN_DELAY);
		addChild(enemy, E_LAYER_ENEMIES);
		enemies_->addObject(enemy);
	}
}

EEnemyFormation GameWorld::GetEnemyFormationType()
{
	// return a formation type from a list of formation types, based on time user has been playing
	// the longer the user has survived, the more difficult the formations will be
	if(seconds_ > E_SKILL6)
	{
		int random_index = CCRANDOM_0_1() * GameGlobals::skill6_formations_size;
		return (EEnemyFormation)(GameGlobals::skill6_formations[random_index]);
	}
	else if(seconds_ > E_SKILL5)
	{
		int random_index = CCRANDOM_0_1() * GameGlobals::skill5_formations_size;
		return (EEnemyFormation)(GameGlobals::skill5_formations[random_index]);
	}
	else if(seconds_ > E_SKILL4)
	{
		int random_index = CCRANDOM_0_1() * GameGlobals::skill4_formations_size;
		return (EEnemyFormation)(GameGlobals::skill4_formations[random_index]);
	}
	else if(seconds_ > E_SKILL3)
	{
		int random_index = CCRANDOM_0_1() * GameGlobals::skill3_formations_size;
		return (EEnemyFormation)(GameGlobals::skill3_formations[random_index]);
	}
	else if(seconds_ > E_SKILL2)
	{
		int random_index = CCRANDOM_0_1() * GameGlobals::skill2_formations_size;
		return (EEnemyFormation)(GameGlobals::skill2_formations[random_index]);
	}
	else if(seconds_ > E_SKILL1)
	{
		int random_index = CCRANDOM_0_1() * GameGlobals::skill1_formations_size;
		return (EEnemyFormation)(GameGlobals::skill1_formations[random_index]);
	}
	else
	{
		return E_FORMATION_RANDOM_EASY;
	}
}

void GameWorld::AddPowerUp()
{
	// limit the number of power-ups on screen
	if(powerups_->count() >= MAX_POWERUPS)
		return;

	PowerUp* powerup = NULL;

	// randomly pick a type of power-up from the power-up frequency array
	int random_index = CCRANDOM_0_1() * GameGlobals::powerup_frequency_size;
	EPowerUpType powerup_type = (EPowerUpType)GameGlobals::powerup_frequency[random_index];

	// create the power-up
	switch(powerup_type)
	{
	case E_POWERUP_BOMB:
		powerup = Bomb::create(this);
		break;
	case E_POWERUP_MISSILE_LAUNCHER:
		powerup = MissileLauncher::create(this);
		break;
	case E_POWERUP_SHIELD:
		powerup = Shield::create(this);
		break;
	default:
		powerup = Bomb::create(this);
	}

	// position it within the boundary & add it
	powerup->setPosition( Vec2(boundary_rect_.origin.x + CCRANDOM_0_1() * boundary_rect_.size.width, boundary_rect_.origin.y + CCRANDOM_0_1() * boundary_rect_.size.height) );
	powerup->Spawn();
	addChild(powerup, E_LAYER_POWERUPS);
	powerups_->addObject(powerup);
}

void GameWorld::AddBlast(Blast* blast)
{
	// add Blast to screen & respective container
	addChild(blast, E_LAYER_BLASTS);
	blasts_->addObject(blast);
}

void GameWorld::AddMissile(Missile* missile)
{
	// add Missile to screen & respective container
	addChild(missile, E_LAYER_MISSILES);
	missiles_->addObject(missile);
}

void GameWorld::update(float dt)
{
	// don't process if player is dying
	if(player_->is_dying_)
		return;

	// update each enemy
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		Enemy* enemy = (Enemy*)object;
		if(enemy)
		{
			enemy->Update(player_->getPosition(), player_->GetShield() == NULL);
		}
	}

	// update each power-up
	object = NULL;
	CCARRAY_FOREACH(powerups_, object)
	{
		PowerUp* powerup = (PowerUp*)object;
		if(powerup)
		{
			powerup->Update();
		}
	}

	CheckCollisions();
	CheckRemovals();
}

void GameWorld::CheckCollisions()
{
	// save player position & radius
	Point player_position = player_->getPosition();
	float player_radius = player_->getRadius();
	
	// iterate through all enemies
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		Enemy* enemy = (Enemy*)object;
		if(enemy)
		{
			Point enemy_position = enemy->getPosition();

			// check with Player
			if(CIRCLE_INTERSECTS_CIRCLE(player_position, player_radius, enemy_position, ENEMY_RADIUS))
			{
				// if shield is enabled, kill enemy
				if(player_->GetShield())
				{
					enemy->Die();
					EnemyKilled();
				}
				// else kill player...but only if enemy has finished spawning
				else if(!enemy->getIsSpawning())
					player_->Die();
			}

			// check with all blasts
			Ref* object2 = NULL;
			CCARRAY_FOREACH(blasts_, object2)
			{
				Blast* blast = (Blast*)object2;
				if(blast)
				{
					if(CIRCLE_INTERSECTS_CIRCLE(blast->getPosition(), blast->getRadius(), enemy_position, ENEMY_RADIUS*1.5f))
					{
						enemy->Die();
						EnemyKilled();
					}
				}
			}

			// check with all missiles
			object2 = NULL;
			CCARRAY_FOREACH(missiles_, object2)
			{
				Missile* missile = (Missile*)object2;
				if(missile)
				{
					if(CIRCLE_INTERSECTS_CIRCLE(missile->getPosition(), MISSILE_RADIUS, enemy_position, ENEMY_RADIUS*1.5f))
					{
						missile->Explode();
					}
				}
			}
		}
	}

	// check if player collides with any of the power-ups
	// activate the power-up if collision is found
	object = NULL;
	CCARRAY_FOREACH(powerups_, object)
	{
		PowerUp* powerup = (PowerUp*)object;
		if(powerup && !powerup->getIsActive())
		{
			if(CIRCLE_INTERSECTS_CIRCLE(player_position, player_radius, powerup->getPosition(), POWERUP_ICON_OUTER_RADIUS))
			{
				powerup->Activate();
			}
		}
	}
}

void GameWorld::CheckRemovals()
{
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		Enemy* enemy = (Enemy*)object;
		if(enemy && enemy->getMustBeRemoved())
		{
			enemies_->removeObject(enemy);
		}
	}

	object = NULL;
	CCARRAY_FOREACH(powerups_, object)
	{
		PowerUp* powerup = (PowerUp*)object;
		if(powerup && powerup->getMustBeRemoved())
		{
			powerups_->removeObject(powerup);
		}
	}

	object = NULL;
	CCARRAY_FOREACH(blasts_, object)
	{
		Blast* blast = (Blast*)object;
		if(blast && blast->getMustBeRemoved())
		{
			blasts_->removeObject(blast);
		}
	}

	object = NULL;
	CCARRAY_FOREACH(missiles_, object)
	{
		Missile* missile = (Missile*)object;
		if(missile && missile->getMustBeRemoved())
		{
			missiles_->removeObject(missile);
		}
	}
}

void GameWorld::Tick(float dt)
{
	// don't tick if player is dying
	if(player_->is_dying_)
		return;

	++ seconds_;

	-- combo_timer_;
	// show the combo achieved if time is up
	if(combo_timer_ < 0)
		combo_timer_ = 0;
	else if(combo_timer_ == 0)
		ComboTimeUp();

	// Tick each enemy
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		Enemy* enemy = (Enemy*)object;
		if(enemy)
		{
			enemy->Tick();
		}
	}

	// Tick each power-up
	object = NULL;
	CCARRAY_FOREACH(powerups_, object)
	{
		PowerUp* powerup = (PowerUp*)object;
		if(powerup)
		{
			powerup->Tick();
		}
	}

	// add an enemy formation every 5 seconds
	if(seconds_ % 5 == 0)
		AddEnemyFormation();
	// add a powerup formation every 4 seconds
	if(seconds_ % 4 == 0)
		AddPowerUp();
}

void GameWorld::EnemyKilled()
{
	// increment counters
	++ enemies_killed_total_;
	++ enemies_killed_combo_;
	// reset combo time
	combo_timer_ = COMBO_TIME;

	// add score & update the label
	score_ += 7;
	score_label_->setString( String::createWithFormat("Score: %d", score_)->getCString() );
}

void GameWorld::ComboTimeUp()
{
	// combo is considered only more than 5 enemies were killed
	if(enemies_killed_combo_ < 5)
		return;

	// add combo to score and update the label
	score_ += enemies_killed_combo_ * 10;
	score_label_->setString( String::createWithFormat("Score: %d", score_)->getCString() );

	// inform the user of the combo by adding a label
	auto combo_label = Label::createWithBMFont("infont.fnt", String::createWithFormat("X%d", enemies_killed_combo_)->getCString());
	combo_label->setPosition(player_->getPositionX(), player_->getPositionY() + combo_label->getContentSize().height);
	combo_label->setScale(0.6f);
	// animate it to move upwards then remove it
	combo_label->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0, 50)), DelayTime::create(0.5f), RemoveSelf::create(true), NULL));
	addChild(combo_label, E_LAYER_HUD);
	// reset combo kill counter
	enemies_killed_combo_ = 0;
}

void GameWorld::PauseGame()
{
	// this prevents multiple pause popups
	if(is_popup_active_)
		return;

	is_popup_active_ = true;

	// pause GameWorld update & Tick functions
	pause();
	
	Vector<Node *> game_world_children = this->getChildren();
	Ref* child = NULL;
	// pause update functions & actions on all GameWorld's children
	for (auto child : game_world_children)
	{
		if(child)
		{
			((Node*)child)->pause();
		}
	}

	// create & add the pause popup
	PausePopup* pause_popup = PausePopup::create(this);
	addChild(pause_popup, E_LAYER_POPUP);
}

void GameWorld::ResumeGame()
{
	is_popup_active_ = false;

	// resume GameWorld update & Tick functions
	resume();
	
	Vector<Node *> game_world_children = this->getChildren();
	Ref* child = NULL;
	// resume update functions & actions on all GameWorld's children
	for(auto child : game_world_children)
	{
		if(child)
		{
			((Node*)child)->resume();
		}
	}
}

void GameWorld::GameOver()
{
	is_popup_active_ = true;

	// stop GameWorld update & Tick functions
	unscheduleAllCallbacks();

	Vector<Node *> game_world_children = this->getChildren();
	Ref* child = NULL;
	// stop update functions & actions on all GameWorld's children
	for (auto child : game_world_children)
	{
		if(child)
		{
			((Node*)child)->unscheduleAllCallbacks();
			((Node*)child)->stopAllActions();
		}
	}

	// display combos if any
	ComboTimeUp();
	// create & add the game over popup
	GameOverPopup* game_over_popup = GameOverPopup::create(this, score_, enemies_killed_total_, seconds_);
	addChild(game_over_popup, E_LAYER_POPUP);
	SOUND_ENGINE->playEffect("game_over.wav");
}

void GameWorld::onAcceleration(Acceleration* acc, Event* event)
{
	HandleInput(ccp(acc->x, acc->y));
}

bool GameWorld::onTouchBegan(const std::vector<Touch*>& set, Event* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Touch* touch = (Touch*)(*set.begin());
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);

	// pause the game if touch is outside the game's boundary
	if(!boundary_rect_.containsPoint(touch_point))
	{
		PauseGame();
		return false;
	}

	// movement controls when running the game on win32
	Point input = Point::ZERO;
	input.x = (touch_point.x - SCREEN_SIZE.width * 0.5f) / (SCREEN_SIZE.width);
	input.y = (touch_point.y - SCREEN_SIZE.height * 0.5f) / (SCREEN_SIZE.height);
	HandleInput(input);
#else
	PauseGame();
#endif	
	return true;
}

void GameWorld::onTouchMoving(const std::vector<Touch*>& set, Event* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	// movement controls when running the game on win32
	Touch* touch = (Touch*)(*set.begin());
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);

	Point input = Point::ZERO;
	input.x = (touch_point.x - SCREEN_SIZE.width * 0.5f) / (SCREEN_SIZE.width);
	input.y = (touch_point.y - SCREEN_SIZE.height * 0.5f) / (SCREEN_SIZE.height);
	HandleInput(input);
#endif
}

void GameWorld::onTouchEnded(const std::vector<Touch*>& set, Event* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	// stop the player when touch has ended
	HandleInput(Point::ZERO);
#endif
}

void GameWorld::HandleInput(Point input)
{
	/// don't accept input if popup is active or if player is dead
	if(is_popup_active_ || player_->is_dying_)
		return;

	Point input_abs = Point(fabs(input.x), fabs(input.y));

	// calculate player speed based on how much device has tilted
	// greater speed multipliers for greater tilt values
	player_->speed_.x = input.x * ( (input_abs.x > 0.3f) ? 36 : ( (input_abs.x > 0.2f) ? 28 : 20 ) );
	player_->speed_.y = input.y * ( (input_abs.y > 0.3f) ? 36 : ( (input_abs.y > 0.2f) ? 28 : 20 ) );

	// update the background
	background_->setPosition( Vec2(input.x * -30, input.y * -30) );
}
