#ifndef _COLORSMASH_GAMEWORLD_SCENE_H_
#define _COLORSMASH_GAMEWORLD_SCENE_H_

#include <iostream>
#include <vector>
#include <math.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

#include "ColourSmashDefinition.h"
#include "..\Resources.h"
#include "MainMenuScene.h"

USING_NS_CC;

class GameWorld : public cocos2d::Layer
{
	// member variable declarations
	// save screenSize for fast access
	Size screenSize;

	// array to represent the colour type for each tile
	std::vector<E_COLOUR_TYPE> tileData;

	// array to hold each tile's sprite
	std::vector<Sprite*> tileSprites;

	// batch rendering
	SpriteBatchNode *spriteBatchNode;

	// arrays to support game logic
	std::vector<int> tilesToRemove;
	std::vector<Sprite*> tilesToShift;

	// score and time
	int score;
	Label *scoreLabel;
	int time;
	Label *timeLabel;

	// buttons and popups
	MenuItemSprite *pauseButton;
	Layer *popup;
	bool isGameOver;

	EventListenerTouchOneByOne *listener;

	public:
		static cocos2d::Scene* createScene();

		virtual bool init();

		// implement the "static create()" method manually
		CREATE_FUNC(GameWorld);

	private:
		void calculateScreensize();
		void createBackground();
		void createTileData();
		void createTileSprites();
		void createTileSprite(int tileId);
		void createHUD();
		void doCountdownAnimation();
		void onPauseClicked(Ref* pSender);
		void drawDebugScreen(Vec2 *vertices);
		void finishCountdownAnimation();
		void updateTimer(float dt);
		void showScoreText(int scoreToAdd, Vec2 point);
		void bringDownTiles();
		void addNewTiles();
		void cleanUpAfterMove();
		bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
		void showPausePopup();
		void onContinueClicked(Ref* pSender);
		void onRestartClicked(Ref* pSender);
		void onMenuClicked(Ref* pSender);
		void showGameOverPopup();

		void findTilesToRemove(int col, int row, E_COLOUR_TYPE tileColour);
		void updateScore(Vec2 point);
		void removeTilesWithAnimation();
		void findTilesToShift();

		Color3B getColourForTile(E_COLOUR_TYPE colourData);
		Point getPositionForTile(int id);

		float TILE_SIZE;
		Vec2 GAMEPLAY_OFFSET;
		float tileZoomFactor;
};

#endif // _COLORSMASH_GAMEWORLD_SCENE_H_
