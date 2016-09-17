#ifndef BACKGROUND_MANAGER_H_
#define BACKGROUND_MANAGER_H_

#include "GameGlobals.h"

class GameWorld;

class BackgroundManager : public Node
{
public:
	virtual bool init();
	~BackgroundManager(void);

	CREATE_FUNC(BackgroundManager);

	void CreateBG();
	void CreateWall();
	void Update(float speed);
	void UpdateBG(float speed);
	void UpdateWall(float speed);

private:
	SpriteBatchNode* batch_node_;
	Sprite* bg_sprite_;
	int num_bg_pieces_;
	__Array* bg_pieces_;
	__Array* wall_sprites_;
	Size bg_piece_size_;

public:
	GameWorld* game_world_;
};

#endif // BACKGROUND_MANAGER_H_
