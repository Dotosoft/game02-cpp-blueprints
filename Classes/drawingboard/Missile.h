#ifndef MISSILE_H_
#define MISSILE_H_

#include "GameGlobals.h"

class GameWorld;

class Missile : public DrawNode
{
public:
	Missile() : target_(Point::ZERO), speed_(Point::ZERO), must_be_removed_(false), has_exploded_(false), game_world_(NULL), previous_position_(Point::ZERO) {};
	virtual ~Missile(){};

	static Missile* createWithTarget(GameWorld* instance, Point target, Point speed);

	virtual bool initWithTarget(GameWorld* instance, Point target, Point speed);
	virtual void update(float dt);
	void Explode(float dt = 0);

	CC_SYNTHESIZE(Point, target_, Target);
	CC_SYNTHESIZE(Point, speed_, Speed);
	CC_SYNTHESIZE(bool, must_be_removed_, MustBeRemoved);
	CC_SYNTHESIZE(bool, has_exploded_, HasExploded);

protected:
	GameWorld* game_world_;
	Point previous_position_;
};

#endif // MISSILE_H_
