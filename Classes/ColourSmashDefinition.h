#ifndef _COLORSMASH_DEFINITION_SCENE_H_
#define _COLORSMASH_DEFINITION_SCENE_H_

#define BORDER_OFFSET 0.1
#define MAX_COLOURS 4												// maximum number of colours we can use
// #define TILE_SIZE 25.0f											// size in points of each tile (same as tile.png)
#define NUM_COLS 14													// maximum number of columns
#define NUM_ROWS 20                          						// maximum number of rows
#define SCORE_PER_TILE 10											// score when a tile is cleared
const int BONUS[] = { 50, 40, 30, 20, 10 };							// number of tiles used to trigger bonuses eg. Bonus if 50 tiles collected in one shot
																	// #define GAMEPLAY_OFFSET cocos2d::Vec2(TILE_SIZE/2, TILE_SIZE)	// offset so that game is not stuck to the bottom-left

// define an object that we can use an enumeration for our colour types
enum E_COLOUR_TYPE {
	E_COLOUR_NONE = 0,
	E_COLOUR_RED = 1,
	E_COLOUR_GREEN = 2,
	E_COLOUR_BLUE = 3,
	E_COLOUR_YELLOW = 4
};

#endif // _COLORSMASH_DEFINITION_SCENE_H_