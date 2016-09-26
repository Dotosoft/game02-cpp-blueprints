#ifndef _CUSTOM_TERRAIN_H_
#define _CUSTOM_TERRAIN_H_

#include "GameGlobals.h"

class CustomTerrain : public Node
{
public:
	CustomTerrain();
	~CustomTerrain();
	static CustomTerrain* create(b2World* world, float start_x);
	virtual bool init(b2World* world, float start_x);

	// virtual void draw();
	CustomCommand _customCommandSprite;
	CustomCommand _customCommandTerrain;
	virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
	virtual void onDrawPrimitives();

	void Update(Point penguin_position);
	void Reset();
	
	void onDrawSpriteCommand(EStripeType stripe_type, int num_stripes);
	Sprite* GenerateStripedSprite(EStripeType stripe_type, int num_stripes);
	void RenderStripes(EStripeType stripe_type, int num_sprites);
	void RenderGradient();
	void RenderHighlight();
	void RenderTopBorder();
	void RenderNoise();

	void GenerateHillKeyPoints(float start_x);
	void GenerateBorderVertices();
	void CreateBody();
	void ResetVertices();

	void SetOffsetX(float offset_x);
	inline float GetOffsetX() { return offset_x_; }
	inline Vec2 GetCliffKeyPoint() { return hill_key_points_[MAX_HILL_KEY_POINTS - 2]; }
	inline Vec2 GetFootKeyPoint() { return hill_key_points_[0]; }

private:
	b2World* world_;
	b2Body* body_;
	Sprite* sprite_;
	float offset_x_;
	bool first_time_;

	int from_key_point_;
	int to_key_point_;
	int prev_from_key_point_;
	int prev_to_key_point_;
	int num_hill_key_points_;
	Vec2 hill_key_points_[MAX_HILL_KEY_POINTS];

	int num_border_vertices_;
	Vec2 border_vertices_[MAX_BORDER_VERTICES];

	int num_hill_vertices_;
	Vec2 hill_vertices_[MAX_HILL_VERTICES];
	Vec2 hill_tex_coords_[MAX_HILL_VERTICES];
};

#endif // _CUSTOM_TERRAIN_H_
