#ifndef GAME_WORLD_H_
#define GAME_WORLD_H_

#include "GameGlobals.h"
#include "CustomTerrain.h"

#ifdef ENABLE_DEBUG_DRAW
class GLESDebugDraw;
#endif

class Sky;
class CustomTerrain;
class Penguin;

class GameWorld : public Layer, public b2ContactListener
{
public:
	GameWorld();
	virtual ~GameWorld();

    static Scene* scene();
    CREATE_FUNC(GameWorld);

    virtual bool init();

#ifdef ENABLE_DEBUG_DRAW
//	virtual void draw();
#endif
	// update functions
	virtual void update(float dt);
	void Tick(float dt);

	// creation functions
	void CreateWorld();
	void DestroyWorld();
	void CreateGame();
	void CreateHUD();

	void AddTime(int value);
	void ShowMessage(const char* message);

	// touch control & contact listener events
	bool onTouchBegan(const std::vector<Touch*>& touches, Event* evt);
	void onTouchEnded(const std::vector<Touch*>& touches, Event* evt);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

	void OnPauseClicked(Ref* sender);
	void ResumeGame();
	void GameOver();

	inline b2World* GetWorld() { return world_; }

private:
	b2World* world_;
#ifdef ENABLE_DEBUG_DRAW
    GLESDebugDraw* debug_draw_;
#endif

	Sky* sky_;
	CustomTerrain* terrain_;
	Penguin* penguin_;
	
	int distance_travelled_;
	Label* distance_label_;
	int time_left_;
	Label* time_label_;
	Label* message_label_;

	bool has_game_begun_;
	bool has_game_ended_;

	EventListenerTouchAllAtOnce *touchListener;

public:
	bool is_popup_active_;
};

#endif // GAME_WORLD_H_
