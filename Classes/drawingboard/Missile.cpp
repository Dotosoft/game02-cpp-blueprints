#include "Missile.h"
#include "GameWorld.h"
#include "Blast.h"

Missile* Missile::createWithTarget(GameWorld* instance, Point target, Point speed)
{
	Missile* missile = new Missile();
	if(missile && missile->initWithTarget(instance, target, speed))
	{
		missile->autorelease();
		return missile;
	}
	CC_SAFE_DELETE(missile);
	return NULL;
}

bool Missile::initWithTarget(GameWorld* instance, Point target, Point speed)
{
	if(!DrawNode::init())
	{
		return false;
	}

	game_world_ = instance;
	target_ = target;
	speed_ = speed;

	// generate vertices for the missile
	Point vertices[] = {Point(MISSILE_RADIUS * 1.75f, 0), Point(MISSILE_RADIUS * -0.875f, MISSILE_RADIUS), Point(MISSILE_RADIUS * -1.75f, 0), Point(MISSILE_RADIUS * -0.875f, MISSILE_RADIUS * -1)};
	// draw a yellow coloured missile
	drawPolygon(vertices, 4, Color4F(0.91765f, 1, 0.14118f, 1), 0, Color4F(0, 0, 0, 0));

	// schedule to explode after 5 seconds
	scheduleOnce(schedule_selector(Missile::Explode), 5.0f);
	scheduleUpdate();

	return true;
}

void Missile::update(float dt)
{
	// find a vector pointing to the target
	Point direction = target_ - getPosition();
	direction.normalize();
	// add the direction to the speed for smooth curved movement
	speed_.x += direction.x;
	speed_.y += direction.y;
	// normalize the speed & multiply with a constant
	speed_.normalize();
	speed_ = speed_ * MISSILE_SPEED;
	setPosition(getPosition().x + speed_.x, getPosition().y + speed_.y);

	// update the rotation of the missile
	Vec2 remainAngle = getPosition() - previous_position_;
	float angle = remainAngle.getAngle();
	setRotation(CC_RADIANS_TO_DEGREES(angle * -1));
	previous_position_ = getPosition();

	// explode the missile if it has roughly reached the target
	if(getPosition().fuzzyEquals(target_, ENEMY_RADIUS * 1.5f))
	{
		Explode();
	}
}

void Missile::Explode(float dt)
{
	// can't expode more than once
	if(has_exploded_)
		return;

	has_exploded_ = true;
	// create three blasts on explosion
	for(int i = 0; i < 3; ++i)
	{
		// create a blast twice the size of the player that should last for quarter of a second
		Blast* blast = Blast::createWithRadiusAndDuration(PLAYER_RADIUS * 2, 0.25f);
		// position it randomly around the missile
		blast->setPosition(getPosition() + Point(CCRANDOM_0_1() * PLAYER_RADIUS * 2 * i, CCRANDOM_0_1() * PLAYER_RADIUS * 2 * i));
		game_world_->AddBlast(blast);
	}
	// remove this missile in the next iteration
	must_be_removed_ = true;
	runAction(Sequence::createWithTwoActions(DelayTime::create(0.01f), RemoveSelf::create(true)));
	SOUND_ENGINE->playEffect("small_blast.wav");
}

