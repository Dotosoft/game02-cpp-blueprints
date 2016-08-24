#ifndef _DRAGONDASH_DEFINITION_SCENE_H_
#define _DRAGONDASH_DEFINITION_SCENE_H_

#define GRAVITY				  -0.75
#define HIGHSCORE_KEY		  "DragonDashHighScore"

enum E_ZORDER {
	E_LAYER_BG				= 0,
	E_LAYER_STARS			= 2,
	E_LAYER_SILHOUETTE		= 4,
	E_LAYER_CASTLE			= 6,
	E_LAYER_TOWER			= 8,
	E_LAYER_PLAYER			= 10,
	E_LAYER_HUD				= 12,
	E_LAYER_POPUPS			= 14
};

#define MAX_SCROLLING_SPEED   6
#define CASTLE_SPRITE_Y		  -50
#define SILHOUETTE_SPRITE_Y   100
#define MAX_STARS			  15

#endif // _DRAGONDASH_DEFINITION_SCENE_H_