#ifndef _SPACECRAZE_POPUPS_H_
#define _SPACECRAZE_POPUPS_H_

#include "spacecraze\GameGlobals.h"
#include "spacecraze\GameWorld.h"
#include "spacecraze\MainMenu.h"
#include "GameResources.h"

namespace spacecraze
{
	class GameWorld;

	class Popup : public Layer
	{
	public:
		Popup();
		~Popup();

		// create & add an empty CCMenu
		virtual Menu* AddMenu();
		// adds a CCMenuItem into the CCMenu
		virtual MenuItem* AddButton(MenuItem* button, Point position);

		// Creates & adds a CCMenuItemLabel using a CCLabelTTF
		virtual MenuItemLabel* AddLabelButton(const char* text, Point position, SEL_MenuHandler handler);
		// Creates & adds a CCMenuItemLabel using a CCLabelBMFont of given font name
		virtual MenuItemLabel* AddLabelButton(const char* text, const char* font, Point position, SEL_MenuHandler handler);
		// Creates & adds a CCMenuItemSprite with specified frame_name
		virtual MenuItemSprite* AddSpriteButton(const char* frame_name, Point position, SEL_MenuHandler handler);

		// button handler functions
		void ResumeGame(Ref* sender);
		void RestartGame(Ref* sender);
		void NextLevel(Ref* sender);
		void QuitToMainMenu(Ref* sender);


	protected:
		GameWorld* game_world_;
		Menu* menu_;
	};

	class PausePopup : public Popup
	{
	public:
		PausePopup(GameWorld* game_world);
		~PausePopup();

		static PausePopup* create(GameWorld* game_world);
	};

	class LevelCompletePopup : public Popup
	{
	public:
		LevelCompletePopup(GameWorld* game_world, int score, int lives);
		~LevelCompletePopup();

		static LevelCompletePopup* create(GameWorld* game_world, int score, int lives);
	};

	class GameOverPopup : public Popup
	{
	public:
		GameOverPopup(GameWorld* game_world, int score);
		~GameOverPopup();

		static GameOverPopup* create(GameWorld* game_world, int score);
	};

}

#endif // _SPACECRAZE_POPUPS_H_
