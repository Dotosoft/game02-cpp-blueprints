#include "Enemy.h"
#include "GameWorld.h"

Enemy* Enemy::create(GameWorld* instance)
{
	Enemy* enemy = new Enemy();
	if(enemy && enemy->init(instance))
	{
		enemy->autorelease();
		return enemy;
	}
	CC_SAFE_DELETE(enemy);
	return NULL;
}

bool Enemy::init(GameWorld* instance)
{
	if(!DrawNode::init())
		return false;

	game_world_ = instance;
	Point vertices[NUM_SPIKES*2];
	GenerateVertices(vertices);	
	
	// draw the star shaped polygon filled with red colour
	drawPolygon(vertices, NUM_SPIKES*2, Color4F(1, 0, 0, 1), 1.5f, Color4F(1, 0, 0, 1));
	// draw a black hole in the middle
	drawDot(Point::ZERO, ENEMY_RADIUS, Color4F(0, 0, 0, 1));

	setScale(0.0f);

	return true;
}

void Enemy::GenerateVertices(Point vertices[])
{
	vector<Point> inner_vertices, outer_vertices;
	// get two regular polygons, one smaller than the other and with a slightly advance rotation
	log("\ninner_vertices");
	GameGlobals::GetRegularPolygonVertices(inner_vertices, NUM_SPIKES, ENEMY_RADIUS);
	log("\nouter_vertices");
	GameGlobals::GetRegularPolygonVertices(outer_vertices, NUM_SPIKES, ENEMY_RADIUS * 1.5f, M_PI / NUM_SPIKES);

	// run a loop to splice the polygons together to form a star
	for(int i = 0; i < NUM_SPIKES; ++i)
	{
		vertices[i*2] = inner_vertices[i];
		vertices[i*2 + 1] = outer_vertices[i];
	}
}

void Enemy::Update(Point player_position, bool towards_player)
{
	// no movement while spawning
	if(is_spawning_)
		return;

	// first find a vector pointing to the player
	Vec2 direction = player_position - getPosition();
	direction.normalize();
	// normalize direction then multiply with the speed_multiplier_
	speed_ = direction * speed_multiplier_ * (towards_player ? 1 : -1);

	// restrict movement within the boundary of the game
	Point next_position = getPosition() + speed_;
	if(RECT_CONTAINS_CIRCLE(game_world_->boundary_rect_, next_position, ENEMY_RADIUS * 1.5f))
	{
		setPosition(next_position);
	}
	else
	{
		if(RECT_CONTAINS_CIRCLE(game_world_->boundary_rect_, Point(next_position.x - speed_.x, next_position.y), ENEMY_RADIUS * 1.5f))
		{
			setPosition(Vec2(next_position.x - speed_.x, next_position.y));
		}
		else if(RECT_CONTAINS_CIRCLE(game_world_->boundary_rect_, Point(next_position.x, next_position.y - speed_.y), ENEMY_RADIUS * 1.5f))
		{
			setPosition(Vec2(next_position.x, next_position.y - speed_.y));
		}
	}
}

void Enemy::Tick()
{
	// no ticking while spawning
	if(is_spawning_)
		return;

	++ time_alive_;

	// as time increases, so does speed
	switch(time_alive_)
	{
	case E_SLOW:
		speed_multiplier_ = 0.5f;
		break;
	case E_MEDIUM:
		speed_multiplier_ = 0.75f;
		break;
	case E_FAST:
		speed_multiplier_ = 1.25f;
		break;
	case E_SUPER_FAST:
		speed_multiplier_ = 1.5f;
		break;
	}
}

void Enemy::Spawn(float delay)
{
	// play a scale-up animation
	is_spawning_ = true;
	runAction(Sequence::create(DelayTime::create(delay), EaseElasticOut::create(ScaleTo::create(1.0f, 1.0f)), CallFunc::create(this, callfunc_selector(Enemy::FinishSpawn)), NULL));
}

void Enemy::FinishSpawn()
{
	is_spawning_ = false;
}

void Enemy::Die()
{
	// shouldn't die once already dead
	if(is_dead_)
		return;
	
	is_dead_ = true;
	// remove this enemy in the next iteration
	must_be_removed_ = true;
	// animate death then remove with cleanup
	runAction(Sequence::createWithTwoActions(Spawn::createWithTwoActions(EaseSineIn::create(ScaleTo::create(0.1f, 0.0f)), EaseSineIn::create(RotateBy::create(0.1f, -90))), RemoveSelf::create(true)));
	SOUND_ENGINE->playEffect("enemy_death.wav");
}
