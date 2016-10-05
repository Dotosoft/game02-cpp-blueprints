#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include "GameGlobals.h"
#ifdef ICEMAN_DEBUG_MODE
	#include "GameWorld.h"
#endif

class GameWorld;

class GameObject : public Sprite
{
public:
	GameObject() : game_world_(NULL),
		type_(E_GAME_OBJECT_NONE), 
		aabb_(Rect::ZERO), 

#ifdef ICEMAN_DEBUG_MODE
		aabb_node_(NULL),
#endif

		speed_(Point::ZERO) {}

	virtual ~GameObject()
	{}

	virtual void SetAABB(Rect aabb)
	{
		aabb_ = aabb;

#ifdef ICEMAN_DEBUG_MODE
		// draw the AABB in debug mode only
		Point vertices[4];
		vertices[0] = Point::ZERO;
		vertices[1] = Vec2(0, aabb_.size.height);
		vertices[2] = Vec2(aabb_.size.width, aabb_.size.height);
		vertices[3] = Vec2(aabb_.size.width, 0);
		
		aabb_node_ = DrawNode::create();
		aabb_node_->drawPolygon(vertices, 4, Color4F(0, 0, 0, 0), 1, Color4F(1, 0, 0, 1));
		aabb_node_->setPosition(aabb_.origin);
		game_world_->addChild(aabb_node_);
#endif
	}

	inline Rect GetAABB() { return aabb_; }
	virtual void SetSpeed(Point speed) { speed_ = speed; }
	inline Point GetSpeed() { return speed_; }

	virtual void Update()
	{
		aabb_.origin.x += speed_.x;
		aabb_.origin.y += speed_.y;

#ifdef ICEMAN_DEBUG_MODE
		aabb_node_->setPosition(getParent()->convertToWorldSpace(aabb_.origin));
#endif
	}
	virtual void CollisionResponse(int tile_col, int tile_row, ECollisionType collision_type) {}

protected:
	GameWorld* game_world_;
	EGameObjectType type_;
	Rect aabb_;
	Point speed_;

#ifdef ICEMAN_DEBUG_MODE
public:
	DrawNode* aabb_node_;
#endif
};

#endif // GAME_OBJECT_H_
