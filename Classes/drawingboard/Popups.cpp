#include "Popups.h"
#include "GameWorld.h"
#include "MainMenu.h"

Popup::Popup()
{
	Sprite* popup_frame = Sprite::createWithSpriteFrameName("iupopup");
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
	return (MenuItemLabel*)AddButton(MenuItemLabel::create(Label::createWithTTF(text, "Comic Sans MS", 36), this, handler), position);
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
	game_world_->ResumeGame();
	runAction(Sequence::createWithTwoActions(EaseBackIn::create(ScaleTo::create(0.25f, 0.0f)), RemoveSelf::create(true)));
}

void Popup::RestartGame(Ref* sender)
{
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

	// add the title/message of the popup
	auto label = Label::createWithBMFont("infont.fnt", "Game Paused");
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.7));
	label->setScale(1.4f);
	addChild(label);

	// create menu & buttons
	AddMenu();
	AddLabelButton("Continue", "infont.fnt", Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.5), menu_selector(Popup::ResumeGame));
	AddLabelButton("Restart", "infont.fnt", Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.4), menu_selector(Popup::RestartGame));
	AddLabelButton("Main Menu", "infont.fnt", Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.3), menu_selector(Popup::QuitToMainMenu));
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

GameOverPopup::GameOverPopup(GameWorld* game_world, int score, int kills, int time)
{
	game_world_ = game_world;

	// add the title/message of the popup
	auto label = Label::createWithBMFont("infont.fnt", "Game Over!");
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.7));
	label->setScale(1.4f);
	addChild(label);

	// add the score
	label = Label::createWithBMFont("infont.fnt", String::createWithFormat("Score: %d", score)->getCString());
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.6));
	addChild(label);

	// add the kills
	label = Label::createWithBMFont("infont.fnt", String::createWithFormat("Enemies Killed: %d", kills)->getCString());
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.55));
	addChild(label);

	// add the time
	int minutes = time/60;
	label = Label::createWithBMFont("infont.fnt", String::createWithFormat("Time: %02d:%02d", minutes, time - minutes * 60)->getCString());
	label->setPosition(Vec2(SCREEN_SIZE.width*0.5, SCREEN_SIZE.height*0.5));
	addChild(label);

	// create menu & buttons
	AddMenu();
	AddLabelButton("Restart", "infont.fnt", Vec2(SCREEN_SIZE.width*0.4, SCREEN_SIZE.height*0.4), menu_selector(Popup::RestartGame));
	AddLabelButton("Main Menu", "infont.fnt", Vec2(SCREEN_SIZE.width*0.6, SCREEN_SIZE.height*0.4), menu_selector(Popup::QuitToMainMenu));
}

GameOverPopup::~GameOverPopup()
{}

GameOverPopup* GameOverPopup::create(GameWorld* game_world, int score, int kills, int time)
{
	GameOverPopup* game_over_popup = new GameOverPopup(game_world, score, kills, time);
	if(game_over_popup && game_over_popup->initWithColor(Color4B(0, 0, 0, 128)))
	{
		game_over_popup->autorelease();
		return game_over_popup;
	}
	CC_SAFE_DELETE(game_over_popup);
	return NULL;
}
