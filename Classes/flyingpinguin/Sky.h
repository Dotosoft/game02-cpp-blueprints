#ifndef SKY_H_
#define SKY_H_

#include "GameGlobals.h"

class Sky : public Node
{
public:
	static Sky* create();
	virtual bool init();
	~Sky();

	Sprite* GenerateSkySprite();
	void Update(Point penguin_position, float terrain_offset_x, float terrain_scale);

private:
	Sprite* sprite_;
	Sprite* clouds_;
	float offset_x_;
};

#endif // SKY_H_
