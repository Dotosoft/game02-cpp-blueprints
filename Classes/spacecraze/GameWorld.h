#ifndef _SPACECRAZE_GAMEWORLD_H_
#define _SPACECRAZE_GAMEWORLD_H_

#include "tinyxml2.h"
#include "GameResources.h"
#include "spacecraze\GameGlobals.h"
#include "spacecraze\BackgroundManager.h"
#include "spacecraze\Player.h"
#include "spacecraze\Enemy.h"
#include "spacecraze\Brick.h"
#include "spacecraze\Popups.h"

using namespace spacecraze;

namespace spacecraze
{
	class Player;

	class GameWorld : public Layer
	{
	public:
		GameWorld();
		virtual ~GameWorld();

		// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
		virtual bool init();

		// there's no 'id' in cpp, so we recommend returning the class instance pointer
		static Scene* scene();

		virtual void update(float dt);

		// implement the "static node()" method manually
		CREATE_FUNC(GameWorld);

		// touch handlers
		// virtual void ccTouchesBegan(CCSet* set, CCEvent* event);
		// virtual void ccTouchesMoved(CCSet* set, CCEvent* event);
		bool onTouchBegan(const std::vector<Touch*>& touches, Event* evt);
		void onTouchMoving(const std::vector<Touch*>& touches, Event* evt);

		// functions that create the level
		void CreateLevel();
		void CreateEnemies(tinyxml2::XMLNode* enemy_set);
		void CreateBricks(tinyxml2::XMLNode* brick_set);
		void CreatePlayer();
		void CreateHUD();

		// game life-cycle functions
		void StartGame();
		void StopGame();
		void ResumeGame();

		// game update functions
		void StartMovingEnemies();
		void Update();
		void CheckCollisions();
		void HandleTouch(Point touch);
		void OnPauseClicked(Ref* sender);

		// level/game end functions
		void GameOver();
		void LevelComplete();
		void Reset();
		void RestartLevel();
		void NextLevel();

		// remove functions
		void RemoveEnemy(Sprite* enemy);
		void RemoveBrick(Sprite* brick);

		// HUD updating functions
		void AddScore(int score_to_add);
		void ReduceLives();

		// Bullet related functions
		void FirePlayerBullet(float dt);
		void FireEnemyBullet(float dt);
		void RemovePlayerBullet(Node* bullet);
		void RemoveEnemyBullet(Node* bullet);

	private:
		SpriteBatchNode* sprite_batch_node_;

		// Player related members
		Player* player_;
		__Array* player_bullets_;
		float player_fire_rate_;

		// Enemy related members
		__Array* enemies_;
		__Array* enemy_bullets_;
		float enemy_movement_duration_;
		float enemy_fire_rate_;
		float left_side_enemy_position_;
		float right_side_enemy_position_;
		Size max_enemy_size_;

		// Array containing Bricks
		__Array* bricks_;

		// HUD elements
		Label* score_label_;
		__Array* life_sprites_;
		int score_;

		// data to carry to next level
		int lives_to_carry_;
		int score_to_carry_;
		// state machine flags
		bool has_game_started_;
		bool has_game_stopped_;
		bool is_game_paused_;

	public:
		// variables accessed by class Popup
		int current_level_;
		bool is_popup_active_;
	};
}

#endif // _SPACECRAZE_GameWorld_SCENE_H_
