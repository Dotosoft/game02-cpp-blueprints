#include "Popups.h"
#include "GameWorld.h"
#include "MainMenu.h"

Popup::Popup()
{
	Sprite* popup_frame = Sprite::create("popup.png");
	popup_frame->setPosition(Vec2(SCREEN_SIZE.width/2, SCREEN_SIZE.height/2));
	addChild(popup_frame);

	// set initial scale & animate the entry of the popup
	setScale(0.0f);
	runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));

	menu_ = NULL;
}

Popup::~Popup()
{}

Menu* Popup::AddMenu()
{
	// return menu_ if already created & added
	if(menu_)
		return menu_;

	// create & add the menu_
	menu_ = Menu::create();
	menu_->setAnchorPoint(Point::ZERO);
	menu_->setPosition(Point::ZERO);
	addChild(menu_);
	return menu_;
}

MenuItem* Popup::AddButton(MenuItem* button, Point position)
{
	// if menu_ or button are NULL, return
	if( (menu_ == NULL && AddMenu() == NULL) || button == NULL )
		return NULL;

	// position the button & add to menu_
	button->setPosition(position);
	menu_->addChild(button);
	return button;
}

MenuItemLabel* Popup::AddLabelButton(const char* text, Point position, SEL_MenuHandler handler)
{
	// create CCMenuItemLabel with CCLabelTTF
	return (MenuItemLabel*)AddButton(MenuItemLabel::create(Label::createWithTTF(text, "Arial", 36.0f), this, handler), position);
}

MenuItemLabel* Popup::AddLabelButton(const char* text, const char* font, Point position, SEL_MenuHandler handler)
{
	// create CCMenuItemLabel with CCLabelBMFont
	return (MenuItemLabel*)AddButton(MenuItemLabel::create(Label::createWithBMFont(font, text), this, handler), position);
}

MenuItemSprite* Popup::AddSpriteButton(const char* frame_name, Point position, SEL_MenuHandler handler)
{
	// create CCMenuItemSprite
	return (MenuItemSprite*)AddButton(MenuItemSprite::create(Sprite::createWithSpriteFrameName(frame_name), Sprite::createWithSpriteFrameName(frame_name), this, handler), position);
}

void Popup::ResumeGame(Ref* sender)
{
	log("Popup::ResumeGame A");
	game_world_->ResumeGame();
	log("Popup::ResumeGame B");
	runAction(Sequence::createWithTwoActions(EaseBackIn::create(ScaleTo::create(0.25f, 0.0f)), RemoveSelf::create(true)));
}

void Popup::RestartGame(Ref* sender)
{
	removeFromParentAndCleanup(true);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}

void Popup::NextLevel(Ref* sender)
{
	++ GameGlobals::level_number_;
	removeFromParentAndCleanup(true);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::scene()));
}

void Popup::QuitToMainMenu(Ref* sender)
{
	removeFromParentAndCleanup(true);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MainMenu::scene()));
}

PausePopup::PausePopup(GameWorld* game_world)
{
	game_world_ = game_world;
	game_world_->is_popup_active_ = true;

	// add the title/message of the popup
	auto label = Label::createWithBMFont("font_01.fnt", "Game Paused");
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.7f));
	addChild(label);

	// create menu & buttons
	AddMenu();
	AddLabelButton("Continue", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.55f), menu_selector(Popup::ResumeGame));
	AddLabelButton("Restart", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.45f), menu_selector(Popup::RestartGame));
	AddLabelButton("Main Menu", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.35f), menu_selector(Popup::QuitToMainMenu));
}

PausePopup::~PausePopup()
{}

PausePopup* PausePopup::create(GameWorld* game_world)
{
	PausePopup* pause_popup = new PausePopup(game_world);
	if(pause_popup && pause_popup->initWithColor(Color4B(0, 0, 0, 128)))
	{
		pause_popup->autorelease();
		return pause_popup;
	}
	CC_SAFE_DELETE(pause_popup);
	return NULL;
}

LevelCompletePopup::LevelCompletePopup(GameWorld* game_world)
{
	game_world_ = game_world;
	game_world_->is_popup_active_ = true;

	// add the title/message of the popup
	auto label = Label::createWithBMFont("font_01.fnt", (GameGlobals::level_number_ >= 5 ? "You Win!" : "Stage Complete!"));
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.7f));
	addChild(label);

	// create menu & buttons
	AddMenu();
	AddLabelButton("Restart", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.45f), menu_selector(Popup::RestartGame));
	AddLabelButton("Main Menu", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.35f), menu_selector(Popup::QuitToMainMenu));
	if(GameGlobals::level_number_ < 5)
	{
		AddLabelButton("Next Level", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.55f), menu_selector(Popup::NextLevel));
	}
}

LevelCompletePopup::~LevelCompletePopup()
{}

LevelCompletePopup* LevelCompletePopup::create(GameWorld* game_world)
{
	LevelCompletePopup* level_complete_popup = new LevelCompletePopup(game_world);
	if(level_complete_popup && level_complete_popup->initWithColor(Color4B(0, 0, 0, 128)))
	{
		level_complete_popup->autorelease();
		return level_complete_popup;
	}
	CC_SAFE_DELETE(level_complete_popup);
	return NULL;
}

GameOverPopup::GameOverPopup(GameWorld* game_world)
{
	game_world_ = game_world;
	game_world_->is_popup_active_ = true;

	// add the title/message of the popup
	auto label = Label::createWithBMFont("font_01.fnt", "Game Over!");
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.7f));
	addChild(label);

	// create menu & buttons
	AddMenu();
	AddLabelButton("Restart", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.5f), menu_selector(Popup::RestartGame));
	AddLabelButton("Main Menu", "font_01.fnt", Vec2(SCREEN_SIZE.width*0.5f, SCREEN_SIZE.height*0.4f), menu_selector(Popup::QuitToMainMenu));

	GameGlobals::level_number_ = 1;
	GameGlobals::hero_lives_left_ = 3;
}

GameOverPopup::~GameOverPopup()
{}

GameOverPopup* GameOverPopup::create(GameWorld* game_world)
{
	GameOverPopup* game_over_popup = new GameOverPopup(game_world);
	if(game_over_popup && game_over_popup->initWithColor(Color4B(0, 0, 0, 128)))
	{
		game_over_popup->autorelease();
		return game_over_popup;
	}
	CC_SAFE_DELETE(game_over_popup);
	return NULL;
}
