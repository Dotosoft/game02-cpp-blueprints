#include "spacecraze\GameWorld.h"

using namespace spacecraze;

GameWorld::GameWorld()
{
	sprite_batch_node_ = NULL;
	player_ = NULL;
	player_bullets_ = NULL;
	player_fire_rate_ = 0.0f;
	enemies_ = NULL;
	enemy_bullets_ = NULL;
	enemy_movement_duration_ = 0.0f;
	enemy_fire_rate_ = 0.0f;
	left_side_enemy_position_ = 0.0f;
	right_side_enemy_position_ = 0.0f;
	max_enemy_size_ = Size::ZERO;
	bricks_ = NULL;
	score_label_ = NULL;
	life_sprites_ = NULL;
	score_ = 0;
	current_level_ = 0;
	lives_to_carry_ = 3;
	score_to_carry_ = 0;
	has_game_started_ = false;
	has_game_stopped_ = false;
	is_popup_active_ = false;
}

GameWorld::~GameWorld()
{}

Scene* GameWorld::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    GameWorld *layer = GameWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	setTouchEnabled(true);
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(GameWorld::onTouchMoving, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// game always starts with first level
	current_level_ = 1;
	// initially lives will be 3 & score will be 0
	lives_to_carry_ = 3;
	score_to_carry_ = 0;
	CreateLevel();

    return true;
}

void GameWorld::update(float dt)
{
	// no collision checking if game has not started OR has stopped OR is paused
	if(!has_game_started_ || has_game_stopped_ || is_game_paused_)
		return;

	CheckCollisions();
}

bool GameWorld::onTouchBegan(const std::vector<Touch*>& touches, Event* evt)
{
	auto touch = (Touch*) touches.at(0);
	auto touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);
	HandleTouch(touch_point);
	return true;
}

void GameWorld::onTouchMoving(const std::vector<Touch*>& touches, Event* evt)
{
	auto touch = (Touch*) touches.at(0);
	auto touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);
	HandleTouch(touch_point);
}

void GameWorld::CreateLevel()
{
	// create the environment
	auto background_manager = BackgroundManager::create();
	addChild(background_manager, E_LAYER_BACKGROUND);

	// create & add the batch node
	sprite_batch_node_ = SpriteBatchNode::create(RESOURCES_SPAZECRAZE_IMAGE_SPACETEX, 128);
	addChild(sprite_batch_node_);

	// initialize score & state machine flags
	score_ = score_to_carry_;
	has_game_started_ = false;
	has_game_stopped_ = false;
	is_game_paused_ = false;
	// initialize enemy position variables
	left_side_enemy_position_ = SCREEN_SIZE.width/2;
	right_side_enemy_position_ = SCREEN_SIZE.width/2;

	// generate level filename
	// char level_file[16] = {0};
	// sprintf(level_file, "spacecraze/Level%02d.xml", current_level_);

	const char * level_file = String::createWithFormat("spacecraze/Level%02d.xml", current_level_)->getCString();

	// fetch level file data
	// unsigned long size;
	ssize_t size;
	// char* data = (char*)CCFileUtils::sharedFileUtils()->getFileData(level_file, "rb", &size);
	char* data = (char*) FileUtils::getInstance()->getFileData(level_file, "rb", &size);

	// parse the level file
	tinyxml2::XMLDocument xml_document;
	tinyxml2::XMLError xml_result = xml_document.Parse(data, size);

	CC_SAFE_DELETE(data);

	// print the error if parsing was unsuccessful
	if(xml_result != tinyxml2::XML_SUCCESS)
	{
		CCLOGERROR("Error:%d while reading %s", xml_result, level_file);
		return;
	}

	// save player data
	tinyxml2::XMLNode* level_node = xml_document.FirstChild();
	player_fire_rate_ = level_node->ToElement()->FloatAttribute("player_fire_rate");

	// create set of enemies
	tinyxml2::XMLNode* enemy_set_node = level_node->FirstChild(); 
	CreateEnemies(enemy_set_node);

	// create set of bricks
	tinyxml2::XMLNode* brick_set_node = enemy_set_node->NextSibling(); 
	CreateBricks(brick_set_node);

	CreatePlayer();

	CreateHUD();

	// everything created, start updating
	scheduleUpdate();
}

void GameWorld::CreateEnemies(tinyxml2::XMLNode* enemy_set)
{
	// save enemy movement & firing information
	enemy_movement_duration_ = enemy_set->ToElement()->FloatAttribute("move_duration");
	enemy_fire_rate_ = enemy_set->ToElement()->FloatAttribute("fire_rate");
	
	// create array to hold enemies
	enemies_ = __Array::create();
	enemies_->retain();

	// create array to hold enemy bullers
	enemy_bullets_ = __Array::createWithCapacity(MAX_BULLETS);
	enemy_bullets_->retain();

	// iterate through <EnemySet> and create Enemy objects
	tinyxml2::XMLElement* enemy_element = NULL;
	for(tinyxml2::XMLNode* enemy_node = enemy_set->FirstChild(); enemy_node != NULL; enemy_node = enemy_node->NextSibling())
	{
		enemy_element = enemy_node->ToElement();
		// Enemy sprite frame name taken from "name" attribute of <Enemy>
		auto enemy = Enemy::createWithSpriteFrameName(this, enemy_element->Attribute("name"));
		// Enemy score taken from "score" attribute of <Enemy>
		enemy->setScore(enemy_element->IntAttribute("score"));
		// add Enemy to batch node & array
		sprite_batch_node_->addChild(enemy, E_LAYER_ENEMIES_BRICKS);
		enemies_->addObject(enemy);

		// Enemy position taken from "position" attribute of <Enemy>
		auto position = GameGlobals::GetPointFromString(string(enemy_element->Attribute("position")));
		enemy->setPosition(position);

		// save enemies at the left & right extremes
		left_side_enemy_position_ = (position.x < left_side_enemy_position_) ? position.x : left_side_enemy_position_;
		right_side_enemy_position_ = (position.x > right_side_enemy_position_) ? position.x : right_side_enemy_position_;

		// save size of largest enemy
		auto size = enemy->getContentSize();
		max_enemy_size_.width = (size.width > max_enemy_size_.width) ? size.width:max_enemy_size_.width;
		max_enemy_size_.height = (size.height > max_enemy_size_.height) ? size.height:max_enemy_size_.height;
	}
}

void GameWorld::CreateBricks(tinyxml2::XMLNode* brick_set)
{
	// create array to hold bricks
	bricks_ = __Array::create();
	bricks_->retain();

	// iterate through <BrickSet> and create Brick objects
	tinyxml2::XMLElement* brick_element = NULL;
	for(tinyxml2::XMLNode* brick_node = brick_set->FirstChild(); brick_node != NULL; brick_node = brick_node->NextSibling())
	{
		brick_element = brick_node->ToElement();
		// Brick sprite frame name taken from "name" attribute of <Brick>
		Brick* brick = Brick::createWithSpriteFrameName(brick_element->Attribute("name"));
		// Brick score taken from "score" attribute of <Brick>
		brick->setScore(brick_element->IntAttribute("score"));
		// Brick position taken from "position" attribute of <Brick>
		brick->setPosition(GameGlobals::GetPointFromString(string(brick_element->Attribute("position"))));
		// add Brick to batch node & array
		sprite_batch_node_->addChild(brick, E_LAYER_ENEMIES_BRICKS);
		bricks_->addObject(brick);
	}
}

void GameWorld::CreatePlayer()
{
	// create & add Player to batch node
	player_ = Player::createWithSpriteFrameName(this, "sfgun");
	sprite_batch_node_->addChild(player_, E_LAYER_PLAYER);

	// create array to hold Player bullets
	player_bullets_ = __Array::createWithCapacity(MAX_BULLETS);
	player_bullets_->retain();

	// initialize Player properties
	player_->setLives(lives_to_carry_);
	player_->setIsRespawning(false);
	// tell Player to move into the screen
	player_->Enter();
}

void GameWorld::CreateHUD()
{
	// create & add "score" text
	auto score_sprite = Sprite::createWithSpriteFrameName("sfscore");
	score_sprite->setPosition(Vec2(SCREEN_SIZE.width*0.15f, SCREEN_SIZE.height*0.925f));
	sprite_batch_node_->addChild(score_sprite, E_LAYER_HUD);

	// create & add "lives" text
	auto lives_sprite = Sprite::createWithSpriteFrameName("sflives");
	lives_sprite->setPosition(Vec2(SCREEN_SIZE.width*0.7f, SCREEN_SIZE.height*0.925f));
	sprite_batch_node_->addChild(lives_sprite, E_LAYER_HUD);

	// create & add score label
	const char * buf = String::createWithFormat("%04d", score_)->getCString();
	score_label_ = Label::createWithBMFont(RESOURCES_SPAZECRAZE_FONT_SFTEXT, buf);
	score_label_->setPosition(Vec2(SCREEN_SIZE.width*0.3f, SCREEN_SIZE.height*0.925f));
	addChild(score_label_, E_LAYER_HUD);

	// save size of life sprite
	auto icon_size = SpriteFrameCache::getInstance()->getSpriteFrameByName("sflifei")->getOriginalSize();
	// create array to hold life sprites
	life_sprites_ = __Array::createWithCapacity(player_->getLives());
	life_sprites_->retain();

	// position life sprites some distance away from "life" text
	float offset_x = lives_sprite->getPositionX() + lives_sprite->getContentSize().width*1.5f + icon_size.width;
	for(int i = 0; i < player_->getLives(); ++i)
	{
		// position each life sprite further away from "life" text
		offset_x -= icon_size.width * 1.5f;
		auto icon_sprite = Sprite::createWithSpriteFrameName("sflifei");
		icon_sprite->setPosition(Vec2( offset_x, SCREEN_SIZE.height*0.925f));
		// add life sprite to batch node & array
		sprite_batch_node_->addChild(icon_sprite, E_LAYER_HUD);
		life_sprites_->addObject(icon_sprite);
	}

	// create & add the pause menu containing pause button)
	auto pause_button = MenuItemSprite::create(Sprite::createWithSpriteFrameName("sfpause"), Sprite::createWithSpriteFrameName("sfpause"), this, menu_selector(GameWorld::OnPauseClicked));
	pause_button->setPosition(Vec2(SCREEN_SIZE.width*0.95f, SCREEN_SIZE.height*0.925f));
	auto menu = Menu::create(pause_button, NULL);
	menu->setAnchorPoint(Point::ZERO);
	menu->setPosition(Point::ZERO);
	addChild(menu, E_LAYER_HUD);
}

void GameWorld::StartGame()
{
	// call this function only once when the game starts
	if(has_game_started_)
		return;

	has_game_started_ = true;
	// start firing player & enemy bullets
	schedule(schedule_selector(GameWorld::FirePlayerBullet), player_fire_rate_);
	schedule(schedule_selector(GameWorld::FireEnemyBullet), enemy_fire_rate_);
	// start moving enemies
	StartMovingEnemies();
}

void GameWorld::StopGame()
{
	has_game_stopped_ = true;
	// stop firing player & enemy bullets
	unschedule(schedule_selector(GameWorld::FirePlayerBullet));
	unschedule(schedule_selector(GameWorld::FireEnemyBullet));

	// stop Enemy movement
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		auto enemy = (Sprite*)object;
		if(enemy)
		{
			enemy->stopAllActions();
		}
	}
}

void GameWorld::ResumeGame()
{
	is_game_paused_ = false;
	is_popup_active_ = false;

	// resume player & enemy bullet firing
	resume();

	// resume Enemy movement
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		auto enemy = (Sprite*) object;
		if(enemy)
		{
			enemy->resume();
		}
	}

	// resume Player bullet movement
	object = NULL;
	CCARRAY_FOREACH(player_bullets_, object)
	{
		auto player_bullet = (Sprite*) object;
		if(player_bullet)
		{
			player_bullet->resume();
		}
	}

	// resume Enemy bullet movement
	object = NULL;
	CCARRAY_FOREACH(enemy_bullets_, object)
	{
		auto enemy_bullet = (Sprite*) object;
		if(enemy_bullet)
		{
			enemy_bullet->resume();
		}
	}
}

void GameWorld::StartMovingEnemies()
{
	// compute maximum distance movable on both sides
	float max_distance_left = left_side_enemy_position_;
	float max_distance_right = SCREEN_SIZE.width - right_side_enemy_position_;
	// compute how much distance to cover per step
	float distance_per_move = max_enemy_size_.width*0.5;
	
	// calculate how many steps on both sides
	int max_moves_left = floor(max_distance_left / distance_per_move);
	int max_moves_right = floor(max_distance_right / distance_per_move);
	int moves_between_left_right = floor( (right_side_enemy_position_ - left_side_enemy_position_) / distance_per_move );

	auto move_left = Sequence::createWithTwoActions(DelayTime::create(enemy_movement_duration_), EaseSineOut::create(MoveBy::create(0.25f, Vec2(distance_per_move*-1, 0))));
	auto move_right = Sequence::createWithTwoActions(DelayTime::create(enemy_movement_duration_), EaseSineOut::create(MoveBy::create(0.25f, Vec2(distance_per_move, 0))));
	auto move_start_to_left = Repeat::create(move_left, max_moves_left);
	auto move_left_to_start = Repeat::create(move_right, max_moves_left);
	auto move_start_to_right = Repeat::create(move_right, max_moves_right);
	auto move_right_to_start = Repeat::create(move_left, max_moves_right);
	auto movement_sequence = Sequence::create(move_start_to_left, move_left_to_start, move_start_to_right, move_right_to_start, NULL);

	// Move each Enemy
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		auto enemy = (Sprite*)object;
		if(enemy)
		{
			enemy->runAction(RepeatForever::create(movement_sequence->clone() ));
		}
	}
}

void GameWorld::CheckCollisions()
{
	Ref* object = NULL;
	Sprite* bullet = NULL;
	bool found_collision = false;

	// collisions between player bullets and bricks & enemies
	CCARRAY_FOREACH(player_bullets_, object)
	{
		bullet = (Sprite*) object;
		if(bullet)
		{
			Rect bullet_aabb = bullet->getBoundingBox();
			
			object = NULL;
			CCARRAY_FOREACH(bricks_, object)
			{
				auto brick = (Sprite*)object;
				// rectangular collision detection between player bullet & brick
				if(brick && bullet_aabb.intersectsRect(brick->getBoundingBox()))
				{
					// on collision, remove brick & player bullet
					RemoveBrick(brick);
					RemovePlayerBullet(bullet);
					found_collision = true;
					break;
				}
			}

			// found collision so stop checking
			if(found_collision)
				break;

			object = NULL;
			CCARRAY_FOREACH(enemies_, object)
			{
				auto enemy = (Sprite*) object;
				// rectangular collision detection between player bullet & enemy
				if(enemy && bullet_aabb.intersectsRect(enemy->getBoundingBox()))
				{
					// on collision, remove enemy & player bullet
					RemoveEnemy(enemy);
					RemovePlayerBullet(bullet);
					found_collision = true;
					break;
				}
			}

			// found collision so stop checking
			if(found_collision)
				break;
		}
	}

	// no collision checking with player when player is respawning
	if(player_->getIsRespawning())
		return;

	// reset variables
	object = NULL;
	bullet = NULL;

	// collisions between enemy bullets and player
	Rect player_aabb = player_->boundingBox();
	CCARRAY_FOREACH(enemy_bullets_, object)
	{
		bullet = (Sprite*)object;
		if(bullet)
		{
			// rectangular collision detection between player & enemy bullet
			if(player_aabb.intersectsRect(bullet->getBoundingBox()))
			{
				// tell player to die, reduce lives & remove enemy
				player_->Die();
				ReduceLives();
				RemoveEnemyBullet(bullet);
				break;
			}
		}
	}
}

void GameWorld::HandleTouch(Point touch)
{
	// don't take touch when a popup is active & when player is respawning
	if(is_popup_active_ || player_->getIsRespawning())
		return;

	player_->setPositionX(touch.x);
}

void GameWorld::OnPauseClicked(Ref* sender)
{
	// this prevents multiple pause popups
	if(is_game_paused_ || is_popup_active_)
		return;

	// pause player & enemy bullet firing
	is_game_paused_ = true;
	pause();

	// pause Enemy movement
	Ref* object = NULL;
	CCARRAY_FOREACH(enemies_, object)
	{
		auto enemy = (Sprite*)object;
		if(enemy)
		{
			enemy->pause();
		}
	}

	// pause Player bullet movement
	object = NULL;
	CCARRAY_FOREACH(player_bullets_, object)
	{
		auto player_bullet = (Sprite*)object;
		if(player_bullet)
		{
			player_bullet->pause();
		}
	}

	// pause Enemy bullet movement
	object = NULL;
	CCARRAY_FOREACH(enemy_bullets_, object)
	{
		auto enemy_bullet = (Sprite*)object;
		if(enemy_bullet)
		{
			enemy_bullet->pause();
		}
	}

	// create & add the pause popup
	auto pause_popup = PausePopup::create(this);
	addChild(pause_popup, E_LAYER_POPUP);
}

void GameWorld::GameOver()
{
	// stop game & reset level variables
	StopGame();
	current_level_ = 1;
	lives_to_carry_ = 3;
	score_to_carry_ = 0;
	// create & add the game over popup
	auto game_over_popup = GameOverPopup::create(this, score_);
	addChild(game_over_popup, E_LAYER_POPUP);
	SOUND_ENGINE->playEffect(RESOURCES_SPAZECRAZE_SOUND_GAMEOVER);
}

void GameWorld::LevelComplete()
{
	// tell player to leave screen
	player_->Leave();
	// stop game & update level variables
	StopGame();
	lives_to_carry_ = player_->getLives();
	score_to_carry_ = score_;
	// create & add the level complete popup
	LevelCompletePopup* level_complete_popup = LevelCompletePopup::create(this, score_, player_->getLives());
	addChild(level_complete_popup, E_LAYER_POPUP);
	SOUND_ENGINE->playEffect(RESOURCES_SPAZECRAZE_SOUND_LVL_COMPLETE);
}

void GameWorld::Reset()
{
	// remove everything here
	player_->removeFromParentAndCleanup(true);
	player_ = NULL;

	player_bullets_->removeAllObjects();
	CC_SAFE_RELEASE_NULL(player_bullets_);

	enemies_->removeAllObjects();
	CC_SAFE_RELEASE_NULL(enemies_);

	enemy_bullets_->removeAllObjects();
	CC_SAFE_RELEASE_NULL(enemy_bullets_);

	bricks_->removeAllObjects();
	CC_SAFE_RELEASE_NULL(bricks_);

	score_label_->removeFromParentAndCleanup(true);
	score_label_ = NULL;

	life_sprites_->removeAllObjects();
	CC_SAFE_RELEASE_NULL(life_sprites_);

	sprite_batch_node_->removeFromParentAndCleanup(true);
	sprite_batch_node_ = NULL;

	is_popup_active_ = false;
	// remove all schedulers & stop all actions
	cleanup();
}

void GameWorld::RestartLevel()
{
	// called from class Popup
	// reset level variables
	current_level_ = 1;
	lives_to_carry_ = 3;
	score_to_carry_ = 0;
	// remove everything
	Reset();
	// create a new level
	CreateLevel();
}

void GameWorld::NextLevel()
{
	// called from class Popup
	// increment level number
	current_level_ = (current_level_ < MAX_LEVELS) ? ++ current_level_ : current_level_;
	// remove everything
	Reset();
	// create a new level
	CreateLevel();
}

void GameWorld::RemoveEnemy(Sprite* enemy)
{
	// remove Enemy from array
	enemies_->removeObject(enemy);
	// tell Enemy to die & credit score
	AddScore(((Enemy*)enemy)->Die());

	// if all enemies are dead, level is complete
	if(enemies_->count() <= 0)
		LevelComplete();
}

void GameWorld::RemoveBrick(Sprite* brick)
{
	// remove Brick from array
	bricks_->removeObject(brick);
	// tell Brick to crumble & credit score
	AddScore(((Brick*)brick)->Crumble());
}

void GameWorld::AddScore(int score_to_add)
{
	// add score & update HUD
	score_ += score_to_add;

	char buf[8] = {0};
	sprintf(buf, "%04d", score_);
	score_label_->setString(buf);
	score_label_->runAction(Sequence::createWithTwoActions(EaseSineIn::create(ScaleTo::create(0.125f, 1.2f)), EaseSineOut::create(ScaleTo::create(0.125f, 1.0f))));
}

void GameWorld::ReduceLives()
{
	if(life_sprites_->count() <= 0)
		return;

	// remove the last life sprite from array
	auto life_sprite = (Sprite*) life_sprites_->getLastObject();
	life_sprites_->removeObject(life_sprite);

	// animate exit of life sprite
	auto scale_down = EaseBackIn::create(ScaleTo::create(0.25f, 0.0f));
	auto remove_self = RemoveSelf::create(true);
	life_sprite->runAction(Sequence::createWithTwoActions(scale_down, remove_self));
}

void GameWorld::FirePlayerBullet(float dt)
{
	// position the bullet slightly above Player
	auto bullet_position = player_->getPosition() + Vec2(0, player_->getContentSize().height * 0.3);

	// create & add the bullet sprite
	auto bullet = Sprite::createWithSpriteFrameName("sfbullet");
	sprite_batch_node_->addChild(bullet, E_LAYER_BULLETS);
	player_bullets_->addObject(bullet);

	// initialize position & scale
	bullet->setPosition(bullet_position);
	bullet->setScale(0.5f);

	// animate the bullet's entry
	auto scale_up = ScaleTo::create(0.25f, 1.0f);
	bullet->runAction(scale_up);

	// move the bullet up
	auto move_up = MoveTo::create(BULLET_MOVE_DURATION, Vec2(bullet_position.x, SCREEN_SIZE.height));
	auto remove = CallFuncN::create(this, callfuncN_selector(GameWorld::RemovePlayerBullet));
	bullet->runAction(Sequence::createWithTwoActions(move_up, remove));

	SOUND_ENGINE->playEffect(RESOURCES_SPAZECRAZE_SOUND_SHOOT_PLAYER);
}

void GameWorld::FireEnemyBullet(float dt)
{
	if(enemies_->count() <= 0)
		return;

	// randomly pick an Enemy & position the bullet slightly below the Enemy
	auto enemy = (Enemy*) enemies_->getRandomObject();
	auto bullet_position = enemy->getPosition() - Vec2(0, enemy->getContentSize().height * 0.3);
	
	// create & add the bullet sprite for this Enemy
	auto bullet = CCSprite::createWithSpriteFrameName(enemy->getBulletName());
	sprite_batch_node_->addChild(bullet, E_LAYER_BULLETS);
	enemy_bullets_->addObject(bullet);

	// initialize position & scale
	bullet->setPosition(bullet_position);
	bullet->setScale(0.5f);

	// animate the bullet's entry
	auto scale_up = ScaleTo::create(0.25f, 1.0f);
	bullet->runAction(scale_up);

	// move the bullet up
	auto move_down = MoveTo::create(enemy->getBulletDuration() * (bullet_position.y / SCREEN_SIZE.height), Vec2(bullet_position.x, 0));
	auto remove = CallFuncN::create(this, callfuncN_selector(GameWorld::RemoveEnemyBullet));
	bullet->runAction(Sequence::createWithTwoActions(move_down, remove));

	SOUND_ENGINE->playEffect(RESOURCES_SPAZECRAZE_SOUND_SHOOT_ENEMY);
}

void GameWorld::RemovePlayerBullet(Node* bullet)
{
	// remove bullet from list & GameWorld
	player_bullets_->removeObject(bullet);
	bullet->removeFromParentAndCleanup(true);
}

void GameWorld::RemoveEnemyBullet(Node* bullet)
{
	// remove bullet from list & GameWorld
	enemy_bullets_->removeObject(bullet);
	bullet->removeFromParentAndCleanup(true);
}
