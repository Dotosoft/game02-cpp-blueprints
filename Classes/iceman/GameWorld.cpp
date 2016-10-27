#include "GameWorld.h"
#include "Popups.h"
#include "Hero.h"
#include "Enemy.h"
#include "Platform.h"

GameWorld::GameWorld()
{
	is_popup_active_ = false;
	tiled_map_ = NULL;
	bricks_layer_ = NULL;
	columns_ = 0;
	rows_ = 0;
	hero_ = NULL;
	num_enemies_ = 0;
	enemies_.clear();
	num_platforms_ = 0;
	platforms_.clear();
	level_complete_height_ = 0.0f;
	life_sprites_ = NULL;
	left_arrow_btn_ = NULL;
	right_arrow_btn_ = NULL;
	jump_btn_ = NULL;
	swing_btn_ = NULL;
	is_left_arrow_pressed_ = false;
	is_right_arrow_pressed_ = false;
	is_jump_pressed_ = false;
	is_swing_pressed_ = false;
}

GameWorld::~GameWorld()
{
	CC_SAFE_RELEASE_NULL(life_sprites_);
}

Scene* GameWorld::scene()
{
    Scene *scene = Scene::create();
    GameWorld *layer = GameWorld::create();
    scene->addChild(layer);
    return scene;
}

bool GameWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }



    CreateGame();
    return true;
}

void GameWorld::CreateGame()
{
	log("GameWorld::CreateGame");
	LayerColor* bg = LayerColor::create(Color4B(25, 49, 69, 255));
	addChild(bg);

	CreateTiledMap();
	CreateControls();
	CreateHUD();
	
	// setTouchEnabled(true);
	this->touchListener = EventListenerTouchAllAtOnce::create();
	this->touchListener->onTouchesBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
	this->touchListener->onTouchesEnded = CC_CALLBACK_2(GameWorld::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(this->touchListener, this);

#ifdef ICEMAN_DEBUG_MODE
	schedule(schedule_selector(GameWorld::update), 0.2f);
#else
	scheduleUpdate();
#endif
}

void GameWorld::CreateTiledMap()
{
	log("GameWorld::CreateTiledMap");
	// generate level filename
	auto buf = String::createWithFormat("level_%02d.tmx", GameGlobals::level_number_);

	// create & add the tiled map
	tiled_map_ = TMXTiledMap::create(buf->getCString());
	addChild(tiled_map_);

	// get the size of the tiled map
	columns_ = (int)tiled_map_->getMapSize().width;
	rows_ = (int)tiled_map_->getMapSize().height;

	// save a reference to the layer containing all the bricks
	bricks_layer_ = tiled_map_->getLayer("Bricks");

	// parse the list of objects
	TMXObjectGroup* object_group = tiled_map_->getObjectGroup("Objects");
	auto objects = object_group->getObjects();
	int num_objects = objects.size();
	
	for(int i = 0; i < num_objects; ++i)
	{
		auto object = objects.at(i).asValueMap();

		// create the Hero at this spawning point
		if(strcmp(object.at("name").asString().c_str(), "HeroSpawnPoint") == 0)
		{
			CreateHero(Vec2(object.at("x").asFloat(), object.at("y").asFloat()));
		}
		// create an Enemy at this spawning point
		else if(strcmp(object.at("name").asString().c_str(), "EnemySpawnPoint") == 0)
		{
			Point position = Vec2(object.at("x").asFloat(), object.at("y").asFloat());
			Point speed = Vec2(object.at("speed_x").asFloat(), object.at("speed_y").asFloat());
			CreateEnemy(position, speed);
		}
		// create a Platform at this spawning point
		else if(strcmp(object.at("name").asString().c_str(), "PlatformSpawnPoint") == 0)
		{
			Point position = Vec2(object.at("x").asFloat(), object.at("y").asFloat());
			Point speed = Vec2(object.at("speed_x").asFloat(), object.at("speed_y").asFloat());
			CreatePlatform(position, speed);
		}
		// save the point where the level should complete
		else if(strcmp(object.at("name").asString().c_str(), "LevelCompletePoint") == 0)
		{
			level_complete_height_ = object.at("y").asFloat();
		}
	}
}

void GameWorld::CreateHero(Point position)
{
	log("GameWorld::CreateHero");
	// create the Hero at the given position
	hero_ = Hero::create(this);
	hero_->setPosition(Vec2(position.x + hero_->getContentSize().width * 0.5f, position.y + hero_->getContentSize().height * 0.5f));
	// set the AABB for the Hero
	hero_->SetAABB(Rect(position.x, position.y, HERO_AABB_WIDTH, HERO_AABB_HEIGHT));
	tiled_map_->addChild(hero_, E_LAYER_HERO);
}

void GameWorld::CreateEnemy(Point position, Point speed)
{
	log("GameWorld::CreateEnemy");
	// create the Enemy at the given position
	Enemy* enemy = Enemy::create(this);
	enemy->setPosition(Vec2(position.x + enemy->getContentSize().width * 0.5f, position.y + enemy->getContentSize().height * 0.5f));
	// set the AABB & speed for this Enemy
	enemy->SetAABB(Rect(position.x, position.y, ENEMY_AABB_WIDTH, ENEMY_AABB_HEIGHT));
	enemy->SetSpeed(speed);
	// push this Enemy into the vector
	enemies_.push_back(enemy);
	++ num_enemies_;
	tiled_map_->addChild(enemy, E_LAYER_ENEMIES);
}

void GameWorld::CreatePlatform(Point position, Point speed)
{
	log("GameWorld::CreatePlatform");
	// create the Platform at the given position
	Platform* platform = Platform::create(this, "moving_platform.png");
	platform->setPosition(Vec2(position.x + platform->getContentSize().width * 0.5f, position.y + platform->getContentSize().height * 0.5f));
	// set the AABB & speed for this Platform
	platform->SetAABB(Rect(position.x, position.y, platform->getContentSize().width, platform->getContentSize().height));
	platform->SetSpeed(speed);
	// push this Platform into the vector
	platforms_.push_back(platform);
	++ num_platforms_;
	tiled_map_->addChild(platform, E_LAYER_PLATFORMS);
}

void GameWorld::CreateControls()
{
	log("GameWorld::CreateControls");
	// create the control sprites
	left_arrow_btn_ = Sprite::create("left_arrow.png");
	right_arrow_btn_ = Sprite::create("right_arrow.png");
	jump_btn_ = Sprite::create("jump.png");
	swing_btn_ = Sprite::create("swing.png");

	// ergonomically place the controls
	left_arrow_btn_->setPosition(Point(100.0f, 150.0f));
	right_arrow_btn_->setPosition(Point(250.0f, 100.0f));
	jump_btn_->setPosition(Point(1180.0f, 150.0f));
	swing_btn_->setPosition(Point(1030.0f, 100.0f));

	// add the controls
	addChild(left_arrow_btn_);
	addChild(right_arrow_btn_);
	addChild(jump_btn_);
	addChild(swing_btn_);
}

void GameWorld::CreateHUD()
{
	log("GameWorld::CreateHUD");
	// create & add the pause button's menu
	Menu* menu = Menu::create();
	menu->setAnchorPoint(Point::ZERO);
	menu->setPosition(Point::ZERO);
	addChild(menu);

	// create & add the pause button
	MenuItemSprite* pause_button = MenuItemSprite::create(Sprite::create("pause_button.png"), Sprite::create("pause_button.png"), this, menu_selector(GameWorld::OnPauseClicked));
	pause_button->setPosition(Vec2(SCREEN_SIZE.width * 0.95f, SCREEN_SIZE.height * 0.9f));
	menu->addChild(pause_button);

	// save size of the life sprite
	Size icon_size = Sprite::create("lives.png")->getSpriteFrame()->getOriginalSize();
	// create an array to hold the life sprites
	life_sprites_ = Array::createWithCapacity(GameGlobals::hero_lives_left_);
	life_sprites_->retain();

	for(int i = 0; i < GameGlobals::hero_lives_left_; ++i)
	{
		Sprite* icon_sprite = Sprite::create("lives.png");
		icon_sprite->setPosition(Vec2(icon_size.width * (i + 1), SCREEN_SIZE.height - icon_size.height));

		// add the life sprite to game world & array
		addChild(icon_sprite, E_LAYER_HUD);
		life_sprites_->addObject(icon_sprite);
	}
}

void GameWorld::update(float dt)
{
	// update the game's main elements
	UpdatePlatforms();
	UpdateEnemies();
	UpdateHero();

	// check for level completion
	if(hero_->GetCurrentGroundHeight() >= level_complete_height_)
	{
		hero_->SetState(E_HERO_STATE_WINNING);
	}

	// scroll the tiled map with some offset
	float curr_y = tiled_map_->getPositionY();
	float next_y = SCREEN_SIZE.height * 0.1f - hero_->GetCurrentGroundHeight();
	tiled_map_->setPositionY( curr_y + (next_y - curr_y) / 5 );

	// remove any dead enemies
	RemoveEnemies();
}

void GameWorld::UpdatePlatforms()
{
	for(int i = 0; i < num_platforms_; ++i)
	{
		platforms_[i]->Update();
	}
}

void GameWorld::UpdateEnemies()
{
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->Update();
	}
}

void GameWorld::UpdateHero()
{
	hero_->Update(is_left_arrow_pressed_, is_right_arrow_pressed_, is_jump_pressed_, is_swing_pressed_);
}

bool GameWorld::CheckCollisions(GameObject* game_object)
{
	bool found_collision = false;
	found_collision = found_collision | CheckVerticalCollisions(game_object);
	found_collision = found_collision | CheckHorizontalCollisions(game_object);
	return found_collision;
}

bool GameWorld::CheckVerticalCollisions(GameObject* game_object)
{
	int visible_rows = (int)tiled_map_->getMapSize().height;
	int visible_cols = (int)tiled_map_->getMapSize().width;

	Rect aabb = game_object->GetAABB();
	Point speed = game_object->GetSpeed();

	// since we're checking vertically, save the row occupied by the aabb
	int aabb_row = GET_ROW_FOR_Y(aabb.origin.y, visible_rows);
	if(speed.y > 0)
	{
		// if we're going up, save the row occupied by the top edge of the aabb
		aabb_row = GET_ROW_FOR_Y(aabb.origin.y + aabb.size.height, visible_rows);
	}
	// also save the columns occupied by the left & right edges of the aabb
	int aabb_start_col = GET_COL_FOR_X(aabb.origin.x);
	int aabb_end_col = GET_COL_FOR_X(aabb.origin.x + aabb.size.width);

	// bounds checking
	if(aabb_row < 0 || aabb_row >= visible_rows ||
			aabb_start_col < 0 || aabb_start_col >= visible_cols ||
			aabb_end_col < 0 || aabb_end_col >= visible_cols)
		return false;

	// initialise flags & counters
	bool found_collidable = false;
	int current_col = aabb_start_col;
	int current_row = aabb_row;

	while(current_row >= 0 && current_row < visible_rows)
	{
		// check for every column that the aabb occupies
		for(current_col = aabb_start_col; current_col <= aabb_end_col; ++current_col)
		{
			// check if a brick exists at the given row & column
			if(bricks_layer_->getTileGIDAt(Vec2(current_col, current_row)))
			{
				found_collidable = true;
				break;
			}
		}

		// from current tile, keep moving in same direction till a brick is found
		if(found_collidable == false)
		{
			current_row = (speed.y < 0) ? (current_row + 1):(current_row - 1);
		}
		else
		{
#ifdef ICEMAN_DEBUG_MODE
			//bricks_layer_->tileAt(Vec2(current_col, current_row))->runAction(Sequence::createWithTwoActions(TintTo::create(0.1f, 255, 0, 0), TintTo::create(0.1f, 255, 255, 255)));
#endif
			break;
		}
	}

	if(found_collidable)
	{
		// going down
		if(speed.y < 0)
		{
			// if the bottom edge of aabb is lower than the top edge of the collidable row
			if(aabb.origin.y <= GET_Y_FOR_ROW(current_row, visible_rows))
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_BOTTOM);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
		// going up
		else
		{
			// if the top edge of aabb is higher than the bottom edge of the collidable row
			if((aabb.origin.y + aabb.size.height) >= GET_Y_FOR_ROW(current_row + 1, visible_rows))
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_TOP);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
	}

	return found_collidable;
}

bool GameWorld::CheckHorizontalCollisions(GameObject* game_object)
{
	int visible_rows = (int)tiled_map_->getMapSize().height;
	int visible_cols = (int)tiled_map_->getMapSize().width;

	Rect aabb = game_object->GetAABB();
	Point speed = game_object->GetSpeed();

	// since we're checking horizontally, save the column occupied by the right edge of aabb
	int aabb_col = GET_COL_FOR_X(aabb.origin.x + aabb.size.width);
	if(speed.x < 0)
	{
		// if we're going left, save the column occupied by the left edge of the aabb
		aabb_col = GET_COL_FOR_X(aabb.origin.x);
	}
	// also save the rows occupied by the top & bottom edges of the aabb
	int aabb_start_row = GET_ROW_FOR_Y(aabb.origin.y + aabb.size.height, visible_rows);
	int aabb_end_row = GET_ROW_FOR_Y(aabb.origin.y, visible_rows);

	// bounds checking
	if(aabb_col < 0 || aabb_col >= visible_cols ||
			aabb_start_row < 0 || aabb_start_row >= visible_rows ||
			aabb_end_row < 0 || aabb_end_row >= visible_rows)
		return false;

	// initialise flags & counters
	bool found_collidable = false;
	int current_col = aabb_col;
	int current_row = aabb_start_row;
	int current_tile = 0;

	while(current_col >= 0 && current_col < visible_cols)
	{
		// check for every row that the aabb occupies
		for(current_row = aabb_start_row; current_row <= aabb_end_row; ++current_row)
		{
			// check if a brick exists at the given row & column
			if(bricks_layer_->getTileGIDAt(Vec2(current_col, current_row)))
			{
				found_collidable = true;
				break;
			}
		}

		// from current tile, keep moving in same direction till a "collidable" tile is found
		if(found_collidable == false)
		{
			current_col = (speed.x < 0) ? (current_col - 1):(current_col + 1);
		}
		else
		{
#ifdef ICEMAN_DEBUG_MODE
			//bricks_layer_->tileAt(Vec2(current_col, current_row))->runAction(Sequence::createWithTwoActions(TintTo::create(0.1f, 255, 0, 0), TintTo::create(0.1f, 255, 255, 255)));
#endif
			break;
		}
	}

	if(found_collidable)
	{
		if(speed.x < 0)
		{
			// if the left edge of aabb is before the right edge of collidable column
			if( aabb.origin.x <= GET_X_FOR_COL(current_col + 1) )
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_LEFT);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
		// going right
		else
		{
			// if the right edge of aabb is after the left edge of collidable column
			if( (aabb.origin.x + aabb.size.width) >= GET_X_FOR_COL(current_col) )
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_RIGHT);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
	}

	return found_collidable;
}

void GameWorld::CheckHeroEnemyCollisions()
{
	Rect hero_aabb = hero_->GetAABB();

	// loop through the list of enemies
	for(int i = 0; i < num_enemies_; ++i)
	{
		// check for collisions between the hero & enemy aabbs
		if(hero_aabb.intersectsRect(enemies_[i]->GetAABB()))
		{
			// if the hero is swinging
			if(hero_->GetState() == E_HERO_STATE_SWINGING)
			{
				// enemy dies
				enemies_[i]->SetState(E_ENEMY_STATE_DYING);
			}
			else
			{
				// hero dies
				ReduceHeroLives();
				hero_->SetState(E_HERO_STATE_DYING);
			}
		}
	}
}

void GameWorld::CheckHeroPlatformCollisions()
{
	Rect hero_aabb = hero_->GetAABB();

	// loop through the list of platforms
	for(int i = 0; i < num_platforms_; ++i)
	{
		Rect platform_aabb = platforms_[i]->GetAABB();
		// check for collisions between the hero & platform aabbs
		if(hero_aabb.intersectsRect(platform_aabb))
		{
			// is the hero below or above the platform
			if(hero_aabb.getMidY() <= platform_aabb.getMidY())
			{
				// reposition the hero one pixel below the platform
				hero_aabb.origin.y = platform_aabb.origin.y - hero_aabb.size.height - 1;
				hero_->SetAABB(hero_aabb);
				// hero should start falling down
				hero_->SetSpeed(Vec2(hero_->GetSpeed().x, GRAVITY));
			}
			else
			{
				// inform the hero that he has landed on a platform
				hero_->SetPlatformBelow(platforms_[i]);
			}
			return;
		}
	}

	// inform the hero that there is no platform below him
	hero_->SetPlatformBelow(NULL);
}

void GameWorld::AddBrick(int tile_col, int tile_row)
{
	// bounds checking
	if(tile_col < 0 || tile_row < 0 || tile_col >= columns_ || tile_row >= rows_)
		return;

	// check if a brick already exists there
	if(bricks_layer_->getTileAt(Vec2(tile_col, tile_row)))
		return;

	// add a brick at the given column & row
	bricks_layer_->setTileGID(1, Vec2(tile_col, tile_row));
}

void GameWorld::RemoveBrick(int tile_col, int tile_row)
{
	bricks_layer_->removeTileAt(Vec2(tile_col, tile_row));
	SOUND_ENGINE->playEffect("brick.wav");
}

void GameWorld::RemoveEnemies()
{
	for(int i = 0; i < num_enemies_; ++i)
	{
		// remove the enemy from the list & the game world
		if(enemies_[i]->GetMustBeRemoved())
		{
#ifdef ICEMAN_DEBUG_MODE
			enemies_[i]->aabb_node_->removeFromParentAndCleanup(true);
#endif
			enemies_[i]->removeFromParentAndCleanup(true);
			enemies_.erase(enemies_.begin() + i);
			-- num_enemies_;
			break;
		}
	}
}

void GameWorld::ReduceHeroLives()
{
	// decrement lives left
	-- GameGlobals::hero_lives_left_;

	// safe checking
	if(GameGlobals::hero_lives_left_ < 0 || life_sprites_->count() <= 0)
	{
		return;
	}

	// remove the last life sprite from array
	Sprite* life_sprite = (Sprite*)life_sprites_->getLastObject();
	life_sprites_->removeObject(life_sprite);

	// animate exit of life sprite
	ActionInterval* scale_down = EaseBackIn::create(ScaleTo::create(0.25f, 0.0f));
	ActionInstant* remove_self = RemoveSelf::create(true);
	life_sprite->runAction(Sequence::createWithTwoActions(scale_down, remove_self));
}

bool GameWorld::onTouchBegan(const std::vector<Touch*>& touches, Event* evt)
{
	Touch* touch = (Touch*) touches.at(0);
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);

	HandleTouch(touch_point, true);

	return true;
}

void GameWorld::onTouchMoved(const std::vector<Touch*>& touches, Event* evt)
{
	Touch* touch = (Touch*) touches.at(0);
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);

	HandleTouch(touch_point, true);
}

void GameWorld::onTouchEnded(const std::vector<Touch*>& touches, Event* evt)
{
	Touch* touch = (Touch*) touches.at(0);
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);

	HandleTouch(touch_point, false);
}

void GameWorld::HandleTouch(Point touch_point, bool is_touching)
{
	is_left_arrow_pressed_ = left_arrow_btn_->getBoundingBox().containsPoint(touch_point) & is_touching;
	is_right_arrow_pressed_ = right_arrow_btn_->getBoundingBox().containsPoint(touch_point) & is_touching;
	is_jump_pressed_ = jump_btn_->getBoundingBox().containsPoint(touch_point) & is_touching;
	is_swing_pressed_ = swing_btn_->getBoundingBox().containsPoint(touch_point) & is_touching;
}

void GameWorld::OnPauseClicked(Ref* sender)
{
	// this prevents multiple pause popups
	if(is_popup_active_)
		return;

	// pause GameWorld update
	pause();
	// setTouchEnabled(false);
	_eventDispatcher->removeEventListener(this->touchListener);
	
	// pause game elements here
	hero_->pause();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->pause();
	}

	// create & add the pause popup
	PausePopup* pause_popup = PausePopup::create(this);
	addChild(pause_popup, E_LAYER_POPUP);
}

void GameWorld::ResumeGame()
{
	is_popup_active_ = false;

	// resume GameWorld update
	resume();
	// setTouchEnabled(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(this->touchListener, this);

	// resume game elements here
	hero_->resume();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->resume();
	}
}

void GameWorld::LevelComplete()
{
	is_popup_active_ = true;

	// stop GameWorld update
	unscheduleAllCallbacks();
	// setTouchEnabled(false);
	_eventDispatcher->removeEventListener(this->touchListener);

	// stop game elements here
	hero_->stopAllActions();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->stopAllActions();
	}

	// create & add the game over popup
	LevelCompletePopup* level_complete_popup = LevelCompletePopup::create(this);
	addChild(level_complete_popup, E_LAYER_POPUP);
}

void GameWorld::GameOver()
{
	is_popup_active_ = true;

	// stop GameWorld update
	unscheduleAllCallbacks();
	// setTouchEnabled(false);
	_eventDispatcher->removeEventListener(this->touchListener);

	// stop game elements here
	hero_->stopAllActions();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->stopAllActions();
	}

	// create & add the game over popup
	GameOverPopup* game_over_popup = GameOverPopup::create(this);
	addChild(game_over_popup, E_LAYER_POPUP);
	SOUND_ENGINE->playEffect("game_over.wav");
}