#ifndef GAME_WORLD_H_
#define GAME_WORLD_H_

#include "GameGlobals.h"

class GameObject;
class Hero;
class Enemy;
class Platform;

class GameWorld : public Layer
{
public:
	GameWorld();
	virtual ~GameWorld();

    static Scene* scene();    
    CREATE_FUNC(GameWorld);

    virtual bool init();

	void CreateGame();
	void CreateTiledMap();
	void CreateHero(Point position);
	void CreateEnemy(Point position, Point speed);
	void CreatePlatform(Point position, Point speed);
	void CreateControls();
	void CreateHUD();

	// update functions
	virtual void update(float dt);
	void UpdatePlatforms();
	void UpdateEnemies();
	void UpdateHero();

	bool CheckCollisions(GameObject* game_object);
	bool CheckVerticalCollisions(GameObject* game_object);
	bool CheckHorizontalCollisions(GameObject* game_object);
	void CheckHeroEnemyCollisions();
	void CheckHeroPlatformCollisions();

	void AddBrick(int tile_col, int tile_row);
	void RemoveBrick(int tile_col, int tile_row);
	void RemoveEnemies();
	void ReduceHeroLives();

	bool onTouchBegan(const std::vector<Touch*>& touches, Event* evt);
	void onTouchMoved(const std::vector<Touch*>& touches, Event* evt);
	void onTouchEnded(const std::vector<Touch*>& touches, Event* evt);
	void HandleTouch(Point touch_point, bool is_touching);

	void OnPauseClicked(Ref* sender);
	void ResumeGame();
	void LevelComplete();
	void GameOver();

	inline TMXTiledMap* GetTiledMap() { return tiled_map_; }
	inline TMXLayer* GetBricksLayer() { return bricks_layer_; }
	inline int GetColumns() { return columns_; }
	inline int GetRows() { return rows_; }

private:
	TMXTiledMap* tiled_map_;
	TMXLayer* bricks_layer_;
	int columns_;
	int rows_;

	Hero* hero_;

	int num_enemies_;
	vector<Enemy*> enemies_;

	int num_platforms_;
	vector<Platform*> platforms_;
	
	float level_complete_height_;

	__Array* life_sprites_;
	Sprite* left_arrow_btn_;
	Sprite* right_arrow_btn_;
	Sprite* jump_btn_;
	Sprite* swing_btn_;
	bool is_left_arrow_pressed_;
	bool is_right_arrow_pressed_;
	bool is_jump_pressed_;
	bool is_swing_pressed_;

	EventListenerTouchAllAtOnce *touchListener;

public:
	// variables accessed by class Popup
	bool is_popup_active_;
};

#endif // GAME_WORLD_H_