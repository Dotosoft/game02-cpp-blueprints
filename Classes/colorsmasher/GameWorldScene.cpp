#include "colorsmasher\GameWorldScene.h"
using namespace colorsmasher;

Scene* GameWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameWorld::init()
{
    //////////////////////////////
    // 1. super init first
	if ( !Layer::init() )
    {
        return false;
    }

	// auto tileSpriteTmp = Sprite::create(RESOURCES_IMAGE_TILE);
    // cocos2d::log("total width ::  %f", tileSpriteTmp->getContentSize().height); // 7.5

    // this->tilesToRemove = Vector<E_COLOUR_TYPE>();
    // this->tilesToShift = Vector<E_COLOUR_TYPE>();
    // myints.size();
    // myints.push_back(i);

	this->calculateScreensize();
    this->createBackground();
	this->createTileData();
	this->createTileSprites();
	this->createHUD();
	this->doCountdownAnimation();

    return true;
}
bool GameWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
	// get touch coordinates
	Vec2 touchLocation = touch->getLocation();
	// calculate touch within the grid
	auto touchWithinGrid = touchLocation - GAMEPLAY_OFFSET;
	// calculate the column touched
	int col = int(floor(touchWithinGrid.x / TILE_SIZE));
	// calculate the row touched
	int row = int(floor(touchWithinGrid.y / TILE_SIZE));
	// calculate the id of the touched tile
	int touchedTile = row * int(NUM_COLS) + col;

	// CCLOG("touchedTile: (%d, %d) - %d", col, row, touchedTile);
	// simple bounds checking to ignore touches outside of the grid
	if (col < 0 || col >= NUM_COLS || row < 0 || row >= NUM_ROWS) {
		return false;
	}

	// CCLOG("Valid touchedTile: %d >>> %d", touchedTile, this->tileData.at(touchedTile));
	// disable touch so that the subsequent functions have time to execute
	this->setTouchEnabled(false);
	this->listener->setEnabled(false);
	this->findTilesToRemove(col, row, this->tileData.at(touchedTile));

	// CCLOG("this->tilesToRemove: %d", this->tilesToRemove.size());
	if (this->tilesToRemove.size() > 1)
	{
		this->updateScore(touch->getLocation());
		this->removeTilesWithAnimation();
		this->findTilesToShift();
	}
	else {
		this->tilesToRemove.clear();
		this->setTouchEnabled(true);
		this->listener->setEnabled(true);
	}
	return true;
}
void GameWorld::updateTimer(float dt)
{
	// this is called every second so reduce the time left by 1
	this->time--;
	// update the time left label
	this->timeLabel->setString(__String::createWithFormat("Time: %d", this->time)->getCString());

	// the user's time is up
	if (this->time <= 0)
	{
		// game is now over
		this->isGameOver = true;
		// unschedule the timer
		this->unschedule(schedule_selector(GameWorld::updateTimer));
		// stop animating the time label
		this->timeLabel->stopAllActions();
		// disable touch
		this->setTouchEnabled(false);
		this->listener->setEnabled(false);
		// disable the pause button
		this->pauseButton->setEnabled(false);
		// display the game over popup
		this->showGameOverPopup();
	}
	else if (this->time == 5)
	{
		// get the user's attention...there are only 5 seconds left
		// make the timer label scale up and down so the user knows the game is about to end
		// use the ease functions to create a heart beat effect
		auto timeUp = Sequence::create(EaseSineIn::create(ScaleTo::create(0.125f, 1.1f)), EaseSineOut::create(ScaleTo::create(0.125f, 1.0f)), NULL);
		// repeat this action forever
		this->timeLabel->runAction(RepeatForever::create(timeUp));
	}
}

#pragma region init_event
void GameWorld::calculateScreensize()
{
	this->screenSize = Director::getInstance()->getWinSize();

	// 1.651613 -> 1.5f
	// 3.303226 -> 2.5f
	// CCLOG("director->setContentScaleFactor: %f", Director::getInstance()->getContentScaleFactor());
	// smallResolutionSize: 320.000000, 480.000000 => 0.575
	// mediumResolutionSize: 768.000000, 1024.000000 => 1.25
	// largeResolutionSize: 1536.000000, 2048.000000 => 2.5
	auto tileWidth = Sprite::create(RESOURCES_IMAGE_TILE)->getContentSize().width;
	float borderOffsetCalc = screenSize.width * BORDER_OFFSET;
	this->TILE_SIZE = (screenSize.width - borderOffsetCalc) / NUM_COLS;
	this->GAMEPLAY_OFFSET = Vec2(borderOffsetCalc / 2, borderOffsetCalc);
	this->tileZoomFactor = TILE_SIZE / tileWidth;
}
void GameWorld::createBackground()
{
	auto background = LayerColor::create(Color4B(25, 0, 51, 255), this->screenSize.width, this->screenSize.height);
	this->addChild(background);

	// generate vertices for the gameplay frame
	Vec2 vertices[4];
	vertices[0] = GAMEPLAY_OFFSET + Vec2(-1, -1);
	vertices[1] = GAMEPLAY_OFFSET + Vec2(-1, (NUM_ROWS * TILE_SIZE) + 1);
	vertices[2] = GAMEPLAY_OFFSET + Vec2((NUM_COLS * TILE_SIZE) + 1, (NUM_ROWS * TILE_SIZE)+1);
	vertices[3] = GAMEPLAY_OFFSET + Vec2((NUM_COLS * TILE_SIZE) + 1, -1); 

	// use new DrawingPrimitive class
	DrawNode *gamePlayFrame = DrawNode::create();
	// pass vertices, fill colour, border width and border colour to get a nice bordered, coloured rectangle
	gamePlayFrame->drawPolygon(vertices, 4, Color4F(0.375, 0.375, 0.375, 1), 2, Color4F(0.4, 0, 0, 1));

	// drawDebugScreen(vertices);

	// must add the DrawNode else it won't be drawn at all
	this->addChild(gamePlayFrame);

	// label to show the title of the game
	auto titleLabel = Label::createWithTTF("ColourSmash", RESOURCES_FONT_COMIC_SANS, 32);
	titleLabel->setPosition( Vec2(this->screenSize.width * 0.5, this->screenSize.height * 0.925) );
	this->addChild(titleLabel);

	// menu containing a button to pause the game
	auto pauseButtonNormal = Sprite::create(RESOURCES_IMAGE_BUTTON_PAUSE);
	auto pauseButtonClicked = Sprite::create(RESOURCES_IMAGE_BUTTON_PAUSE);
	this->pauseButton = MenuItemSprite::create(pauseButtonNormal, pauseButtonClicked, this, menu_selector(GameWorld::onPauseClicked));
	this->pauseButton->setPosition( Vec2(this->screenSize.width * 0.875, this->screenSize.height * 0.925));
	this->pauseButton->setEnabled(false);
	this->pauseButton->setScale(this->tileZoomFactor);
	auto pauseMenu = Menu::create(this->pauseButton, NULL);
	pauseMenu->setPosition( Vec2::ZERO );
	this->addChild( pauseMenu, 1 );
}
void GameWorld::createTileData()
{
	// CCLOG("createTileData");
	this->tileData.clear();
	// generate tile data randomly
	for (int i = 0; i < (NUM_COLS * NUM_ROWS); ++i)
	{
		int indexColorRandom = random(1, MAX_COLOURS);;
		// CCLOG("colorRandom(%d): %d", i, randomIndex);
		this->tileData.push_back(E_COLOUR_TYPE(indexColorRandom));
	}
}
void GameWorld::createTileSprites()
{
	// CCLOG("createTileSprites");
	// create the batch node passing in path to the texture & initial capacity
	// initial capacity is slightly more than maximum number of sprites
	// this is because new tiles may be added before old tiles are removed
	this->spriteBatchNode = SpriteBatchNode::create(RESOURCES_IMAGE_TILE, NUM_COLS * NUM_ROWS + NUM_ROWS);

	for (int i = 0; i < (NUM_COLS * NUM_ROWS); i++)
	{
		this->createTileSprite(i);
	}

	this->addChild(this->spriteBatchNode);
}
void GameWorld::createTileSprite(int tileId)
{
	// CCLOG("createTileSprites");
	// create sprite with the image
	auto tileTmp = Sprite::create(RESOURCES_IMAGE_TILE);
	tileTmp->setScale(this->tileZoomFactor);
	// set colour based on the tile's data
	E_COLOUR_TYPE colorType = this->tileData.at(tileId);
	Color3B colorTmp = this->getColourForTile(colorType);
	tileTmp->setColor(colorTmp);
	// set colour based on the tile's index
	tileTmp->setPosition(getPositionForTile(tileId));
	// save the index of the tile as user data
	// tileTmp->setUserData(&tileId);
	tileTmp->setUserData(Integer::create(tileId));
	// add the sprite to the batch node
	spriteBatchNode->addChild(tileTmp);

	// for debug purpose
	/* auto label = Label::createWithTTF(__String::createWithFormat("%d", tileId)->getCString(), "fonts/Comic Sans MS.ttf", 8);
	label->setPosition(getPositionForTile(tileId));
	this->addChild(label, 12); */

	if (this->tileSprites.size() <= tileId)
	{
		this->tileSprites.push_back(tileTmp);
	}
	else
	{
		this->tileSprites.at(tileId) = tileTmp;
	}
}
void GameWorld::createHUD()
{
	// CCLOG("createHUD");
	// initialize score and time
	this->score = 0;
	this->time = 60;

	// create labels for score and time
	this->scoreLabel = Label::createWithTTF(__String::createWithFormat("Score: %d", this->score)->getCString(), RESOURCES_FONT_COMIC_SANS, 18);
	this->scoreLabel->setPosition(Point(this->screenSize.width * 0.33f, this->screenSize.height * 0.875f));
	this->addChild(this->scoreLabel, 1);

	this->timeLabel = Label::createWithTTF(__String::createWithFormat("Time: %d", this->time)->getCString(), RESOURCES_FONT_COMIC_SANS, 18);
	this->timeLabel->setPosition(Point(this->screenSize.width * 0.66f, this->screenSize.height * 0.875f));
	this->addChild(this->timeLabel, 1);
}
void GameWorld::doCountdownAnimation()
{
	// CCLOG("doCountdownAnimation");
	// create the four labels
	std::vector <Label*> labels;
	for (int i = 0; i<4;i++)
	{
		auto label = Label::createWithTTF("", RESOURCES_FONT_COMIC_SANS, 52);
		// position the label at the centre of the screen
		label->setPosition(Point(this->screenSize.width / 2, this->screenSize.height / 2));
		// reduce opacity so that the label is invisible
		label->setOpacity(0);
		// enlarge the label
		label->setScale(3);

		switch (i)
		{
		case 0:
			label->setString("3");
			break;
		case 1:
			label->setString("2");
			break;
		case 2:
			label->setString("1");
			break;
		case 3:
			label->setString("Start");
			break;
		}

		this->addChild(label);
		labels.push_back(label);
	}

	// fade in and scale down at the same time
	auto fadeInScaleDown = Spawn::create(
		FadeIn::create(0.25f),
		EaseBackOut::create(ScaleTo::create(0.25f, 1)),
		NULL
	);
	// stay on screen for a bit
	auto waitOnScreen = DelayTime::create(0.75f);
	// remove label and cleanup
	auto *removeSelf = RemoveSelf::create(true);

	for (int i = 0;i < 4;++i)
	{
		// since the labels should appear one after the other,
		// we give them increasing delays before they appear
		auto delayBeforeAppearing = DelayTime::create(i);
		auto countDownAnimation = Sequence::create(delayBeforeAppearing, fadeInScaleDown, waitOnScreen, removeSelf, NULL);
		labels.at(i)->runAction(countDownAnimation);
	}

	// after the animation has finished, start the game
	auto waitForAnimation = DelayTime::create(4.0f);
	auto finishCountdownAnimation = CallFunc::create(this, callfunc_selector(GameWorld::finishCountdownAnimation));
	this->runAction(Sequence::create(waitForAnimation, finishCountdownAnimation, NULL));
}
void GameWorld::finishCountdownAnimation()
{
	// CCLOG("finishCountdownAnimation");
	// start executing the game timer
	this->schedule(schedule_selector(GameWorld::updateTimer), 1);
	// finally allow the user to touch
	this->setTouchEnabled(true);
	this->pauseButton->setEnabled(true);

	this->listener = EventListenerTouchOneByOne::create();
	this->listener->onTouchBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
#pragma endregion init_event

#pragma region touch_event
void GameWorld::findTilesToRemove(int col, int row, E_COLOUR_TYPE tileColour)
{
	// CCLOG("findTilesToRemove");
	// first do bounds checking
	if (col < 0 || col >= NUM_COLS || row < 0 || row >= NUM_ROWS)
		return;

	// calculate the ID of the tile using col & row
	int tileId = (row * int(NUM_COLS)) + col;

	// now check if tile is of required colour
	if (this->tileData.at(tileId) != tileColour)
		return;

	// check if tile is already saved
	if (std::find(this->tilesToRemove.begin(), this->tilesToRemove.end(), tileId) != this->tilesToRemove.end()) return;
	// if (this->tilesToRemove.at(tileId) >= 0)
	// if (this->tilesToRemove.at(tileId) <= 0) return;

	// save the tile to be removed
	//CCLOG("findTilesToRemove: %d >>> %d", tileId, tileColour);
	this->tilesToRemove.push_back(tileId);

	// check up
	this->findTilesToRemove(col, row + 1, tileColour);

	// check down
	this->findTilesToRemove(col, row - 1, tileColour);

	// check left
	this->findTilesToRemove(col - 1, row, tileColour);

	// check right
	this->findTilesToRemove(col + 1, row, tileColour);
}
void GameWorld::updateScore(Vec2 point)
{
	// CCLOG("updateScore");
	// count the number of tiles the user just removed
	int numTiles = this->tilesToRemove.size();

	// calculate score for this move
	int scoreToAdd = numTiles * int(SCORE_PER_TILE);

	// check if a bonus has been achieved
	for (int i = 0; i < sizeof(BONUS); ++i)
	{
		if (numTiles >= BONUS[i])
		{
			// add the bonus to the score for this move
			scoreToAdd += BONUS[i] * 20;
			break;
		}
	}

	// display the score for this move
	this->showScoreText(scoreToAdd, point);
	// add the score for this move to the total score
	this->score += scoreToAdd;
	// update the total score label
	this->scoreLabel->setString(__String::createWithFormat("Score: %d", this->score)->getCString());
	// run a simple action so the user knows the score is being added
	// use the ease functions to create a heart beat effect
	this->scoreLabel->runAction(
		Sequence::create(
			EaseSineIn::create(ScaleTo::create(0.125f, 1.1f)),
			EaseSineOut::create(ScaleTo::create(0.125f, 1.0f)),
			NULL)
	);
}
/* this function can be used to display any message to the user but we will use it to display the score for each move */
void GameWorld::showScoreText(int scoreToAdd, Vec2 point) {
	// CCLOG("showScoreText");
	// create the label with the score & place it at the respective point
	auto bonusLabel = Label::createWithTTF(__String::createWithFormat("+%d", scoreToAdd)->getCString(), RESOURCES_FONT_COMIC_SANS, 32);
	bonusLabel->setPosition(point);
	// initially scale it down completely
	bonusLabel->setScale(0);
	// give it a yellow colour
	bonusLabel->setColor(Color3B::YELLOW);
	this->addChild(bonusLabel, 10);

	// animate the bonus label so that it scales up with a nice easing effect
	bonusLabel->runAction(
		Sequence::create(
			EaseBackOut::create(ScaleTo::create(0.125f, 1.0f)),
			DelayTime::create(1),                                   // it should stay on screen so the user can read it
			EaseBackIn::create(ScaleTo::create(0.125f, 0.0f)),      // scale it back down with a nice easing effect
			RemoveSelf::create(true), NULL)							// its task is finished, so remove it with cleanup
	);
}
void GameWorld::removeTilesWithAnimation()
{
	// CCLOG("removeTilesWithAnimation");
	for (int indexToRemove : this->tilesToRemove)
	{
		// first clear the tile's data
		this->tileData.at(indexToRemove) = E_COLOUR_TYPE::E_COLOUR_NONE;
		// std::replace(this->tileData.begin(), this->tileData.end(), this->tileData.at(indexToRemove), E_COLOUR_TYPE::E_COLOUR_NONE);

		// CCLOG("removeTilesWithAnimation: %d", indexToRemove);
		// the tile should scale down with easing and then remove itself
		this->tileSprites.at(indexToRemove)->runAction(
			Sequence::create(
				EaseBackIn::create(ScaleTo::create(0.25f, 0.0f)),
				RemoveSelf::create(true),
				NULL)
		);

		// nullify the tile's sprite
		this->tileSprites.at(indexToRemove) = NULL;
		// this->tileSprites.erase(this->tileSprites.begin() + this->tilesToRemove[i]);
		// this->tileSprites.erase(std::remove(this->tileSprites.begin(), this->tileSprites.end(), this->tileSprites.at(this->tilesToRemove[i])), this->tileSprites.end());
		// std::replace(this->tilesToRemove.begin(), this->tilesToRemove.end(), this->tilesToRemove[i], NULL);
		// this->tilesToRemove.erase(this->tilesToRemove.begin() + i);
	}

	// wait for the scale down animation to finish then bring down the tiles from above
	this->spriteBatchNode->runAction(
		Sequence::create(
			DelayTime::create(0.25), CallFunc::create(this, callfunc_selector(GameWorld::bringDownTiles)),
			NULL)
	);
}
void GameWorld::bringDownTiles() {
	// CCLOG("bringDownTiles");
	for (auto spriteToShift : this->tilesToShift)
	{
		// int spriteDataShift = int(spriteToShift->getUserData());
		int spriteDataShift = ((Integer *)spriteToShift->getUserData())->getValue();
		// the tiles should move to their new positions with an awesome looking bounce
		spriteToShift->runAction(
			Sequence::create(
				EaseBounceOut::create(MoveTo::create(0.2f, this->getPositionForTile(spriteDataShift))),
				NULL
			)
		);
	}

	// wait for the movement to finish then add new tiles
	this->spriteBatchNode->runAction(
		Sequence::create(
			DelayTime::create(0.25f),
			CallFunc::create(this, callfunc_selector(GameWorld::addNewTiles)),
			NULL)
	);
};
void GameWorld::addNewTiles() {
	// CCLOG("addNewTiles");
	// first search for all tiles having value -1...-1 means empty
	std::vector<int> emptyTileIndices;
	std::vector<E_COLOUR_TYPE>::iterator it = std::find(this->tileData.begin(), this->tileData.end(), E_COLOUR_TYPE::E_COLOUR_NONE);
	while (it != this->tileData.end()) {
		size_t index = std::distance(this->tileData.begin(), it);
		// CCLOG("Element found in myvector: %d", index);
		emptyTileIndices.push_back(index);
		it = std::find(++it, this->tileData.end(), E_COLOUR_TYPE::E_COLOUR_NONE);
	}

	// now create tile data and sprites
	for (int emptyIndex : emptyTileIndices)
	{
		// generate tile data randomly
		int indexColorRandom = cocos2d::random(1, MAX_COLOURS);
		this->tileData.at(emptyIndex) = E_COLOUR_TYPE(indexColorRandom);
		// create tile sprite based on tile data
		this->createTileSprite(emptyIndex);
	}

	// animate the entry of the sprites
	for (int emptyIndex : emptyTileIndices)
	{
		// set the scale to 0
		this->tileSprites.at(emptyIndex)->setScale(0);
		// scale the sprite up with a neat easing effect
		this->tileSprites.at(emptyIndex)->runAction(
			Sequence::create(EaseBackOut::create(ScaleTo::create(0.125f, this->tileZoomFactor)), NULL)
		);
	}

	// the move has finally finished, do some cleanup
	this->cleanUpAfterMove();
}
void GameWorld::cleanUpAfterMove() {
	// CCLOG("cleanUpAfterMove");
	// empty the arrays
	this->tilesToRemove.clear();
	this->tilesToShift.clear();
	// enable touch so the user can continue playing, but only if the game isn't over
	if (this->isGameOver == false)
	{
		this->setTouchEnabled(true);
		this->listener->setEnabled(true);
	}
}
void GameWorld::findTilesToShift()
{
	// CCLOG("findTilesToShift");
	// first sort the tiles to be removed, in descending order
	// this->tilesToRemove.sort(function(a, b) { return b - a });
	// std::sort(this->tilesToRemove.begin(), this->tilesToRemove.end()); // Sorting asc
	std::sort(this->tilesToRemove.rbegin(), this->tilesToRemove.rend());   // note: reverse iterators

																		   // for each tile, bring down all the tiles belonging to the same column that are above the current tile
	for (int indexToRemove : this->tilesToRemove)
	{
		// calculate column and row for the current tile to be removed
		int col = int(floor(indexToRemove % NUM_COLS));
		int row = int(floor(indexToRemove / NUM_COLS));

		// CCLOG("Process: %d", indexToRemove);

		// iterate through each row above the current tile
		for (int j = row + 1; j < int(NUM_ROWS); ++j)
		{
			int indexChecked = int((j - 1) * int(NUM_COLS) + col);
			int indexToShift = int((j * int(NUM_COLS)) + col);

			// CCLOG("%d >> Switch tile = %d to %d", j, indexToShift, indexChecked);
			// each tile gets the data of the tile exactly above it
			this->tileData.at(indexChecked) = this->tileData.at(indexToShift);
			// each tile now refers to the sprite of the tile exactly above it
			this->tileSprites.at(indexChecked) = this->tileSprites.at(indexToShift);
			// null checking...this sprite may have already been nullified by removeTilesWithAnimation
			if (this->tileSprites.at(indexChecked))
			{
				// save the new index as user data
				// int userData = (j - 1) * NUM_COLS + col;
				// this->tileSprites.at(indexChecked)->setUserData(new Value(indexChecked));
				this->tileSprites.at(indexChecked)->setUserData(Integer::create(indexChecked));

				// save this tile's sprite so that it is animated, but only if it hasn't already been saved
				// if (this->tilesToShift.indexOf(this->tileSprites[(j - 1) * NUM_COLS + col]) == -1)
				auto spriteToFind = this->tileSprites.at(indexChecked);
				// int indexDataSprite = (*(Value *)spriteToFind->getUserData()).asInt();
				// int indexDataSprite = ((Integer *) spriteToFind->getUserData())->getValue();
				if (std::find(this->tilesToShift.begin(), this->tilesToShift.end(), spriteToFind) == this->tilesToShift.end())
					this->tilesToShift.push_back(spriteToFind);
			}
		}
		// after shifting the whole column down, the tile at the top of the column will be empty
		// set the data to -1...-1 means empty
		int indexTop = (int(NUM_ROWS) - 1) * int(NUM_COLS) + col;
		this->tileData.at(indexTop) = E_COLOUR_TYPE::E_COLOUR_NONE;
		// nullify the sprite's reference
		this->tileSprites.at(indexTop) = NULL;
		// CCLOG("Empty: %d", indexTop);
	}
}
#pragma endregion touch_event

#pragma region menu_event
void GameWorld::onPauseClicked(Ref* pSender)
{
	// pause the time label if it is animating
	this->timeLabel->pauseSchedulerAndActions();
	// disable touch
	this->setTouchEnabled(false);
	// this will pause all schedulers and actions associated with the GameWorld layer
	this->pauseSchedulerAndActions();
	// disable the pause button
	this->pauseButton->setEnabled(false);
	// display the pause popup
	this->showPausePopup();
}
void GameWorld::showPausePopup() {
	// create a black semi-transparent layer
	this->popup = LayerColor::create(Color4B(0, 0, 0, 196), this->screenSize.width, this->screenSize.height);
	// set opacity so that it is not visible
	this->popup->setOpacity(0);
	// fade it in
	this->popup->runAction(FadeTo::create(0.25, 196));
	this->addChild(this->popup, 10);

	// create the continue button
	auto continueButton = MenuItemLabel::create(Label::createWithTTF("Continue", RESOURCES_FONT_COMIC_SANS, 32), this, menu_selector(GameWorld::onContinueClicked));
	continueButton->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.6f));

	// create the restart button
	auto restartButton = MenuItemLabel::create(Label::createWithTTF("Restart", RESOURCES_FONT_COMIC_SANS, 32), this, menu_selector(GameWorld::onRestartClicked));
	restartButton->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.5f));

	// create the menu button
	auto menuButton = MenuItemLabel::create(Label::createWithTTF("Menu", RESOURCES_FONT_COMIC_SANS, 32), this, menu_selector(GameWorld::onMenuClicked));
	menuButton->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.4f));

	// create the pause menu with the above three button
	auto pauseMenu = Menu::create(continueButton, restartButton, menuButton, NULL);
	pauseMenu->setPosition(Vec2::ZERO);
	this->popup->addChild(pauseMenu);

	// title to inform the user which popup this is
	auto pausedLabel = Label::createWithTTF("Game Paused", RESOURCES_FONT_COMIC_SANS, 52);
	pausedLabel->setPosition(Vec2(this->screenSize.width*0.5, this->screenSize.height*0.75));
	this->popup->addChild(pausedLabel);
}
void GameWorld::onContinueClicked(Ref* pSender)
{
	// remove the popup
	this->popup->removeFromParent();
	this->popup = NULL;

	// resume GameWorld's schedulers and actions
	this->resumeSchedulerAndActions();
	// enable touch
	this->setTouchEnabled(true);
	// enable pause button
	this->pauseButton->setEnabled(true);
	// resume the time label's actions
	this->timeLabel->resumeSchedulerAndActions();
}
void GameWorld::onRestartClicked(Ref* pSender)
{
	// replace the scene with a new instance of GameWorldScene...and do this with a transition
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::createScene()));
}
void GameWorld::onMenuClicked(Ref* pSender)
{
	// replace the scene with the MainMenuScene...and do this with a transition
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MainMenu::createScene()));
}
void GameWorld::showGameOverPopup() {
	// create a black semi-transparent layer
	this->popup = LayerColor::create(Color4B(0, 0, 0, 196), this->screenSize.width, this->screenSize.height);
	// set opacity so that it is not visible
	this->popup->setOpacity(0);
	// fade it in
	this->popup->runAction(FadeTo::create(0.25f, 196));
	this->addChild(this->popup, 10);

	// create the restart button
	auto restartButton = MenuItemLabel::create(Label::createWithTTF("Restart", RESOURCES_FONT_COMIC_SANS, 32), this, menu_selector(GameWorld::onRestartClicked));
	restartButton->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.5f));

	// create the menu button
	auto menuButton = MenuItemLabel::create(Label::createWithTTF("Menu", RESOURCES_FONT_COMIC_SANS, 32), this, menu_selector(GameWorld::onMenuClicked));
	menuButton->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.4f));

	// create the pause menu with the above three button
	auto gameOverMenu = Menu::create(restartButton, menuButton, NULL);
	gameOverMenu->setPosition(Vec2::ZERO);
	this->popup->addChild(gameOverMenu);

	// title to inform the user which popup this is
	auto gameOverLabel = Label::createWithTTF("Game Over", RESOURCES_FONT_COMIC_SANS, 52);
	gameOverLabel->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.75f));
	this->popup->addChild(gameOverLabel);

	// add a label to show the final score
	auto scoreLabel = Label::createWithTTF(__String::createWithFormat("Score: %d", this->score)->getCString(), RESOURCES_FONT_COMIC_SANS, 48);
	scoreLabel->setPosition(Vec2(this->screenSize.width*0.5f, this->screenSize.height*0.6f));
	// animate it with a nice heart beat effect to draw the user's attention
	scoreLabel->runAction(
		Sequence::create(DelayTime::create(0.5f),
			EaseSineIn::create(ScaleTo::create(0.25f, 1.1f)),
			EaseSineOut::create(ScaleTo::create(0.25f, 1)),
			NULL
		)
	);
	this->popup->addChild(scoreLabel);
}
#pragma endregion menu_event

#pragma region helper
Color3B GameWorld::getColourForTile(E_COLOUR_TYPE colourData) {
	// CCLOG("getColourForTile");
	// return a colour for the specified type
	auto colorResult = Color3B::WHITE;
	switch (colourData)
	{
	case E_COLOUR_RED:
		colorResult = Color3B(255, 128, 0);
		break;
	case E_COLOUR_GREEN:
		colorResult = Color3B(0, 204, 0);
		break;
	case E_COLOUR_BLUE:
		colorResult = Color3B(0, 76, 153);
		break;
	case E_COLOUR_YELLOW:
		colorResult = Color3B(102, 0, 102);
		break;
	}

	return colorResult;
}
Point GameWorld::getPositionForTile(int id) {
	// CCLOG("getPositionForTile");
	// calculate column and row for given tile id
	float col = floor(id % NUM_COLS);
	float row = floor(id / NUM_COLS);
	// return point based on the column and row of the tile
	return GAMEPLAY_OFFSET + Point(col * TILE_SIZE + TILE_SIZE / 2, row * TILE_SIZE + TILE_SIZE / 2);
}
void GameWorld::drawDebugScreen(Vec2 *vertices) {
	// Debug Drawing
	int lengthOfVertice = sizeof(vertices);
	// CCLOG("drawDebugScreen: %d", lengthOfVertice);
	for (int i = 0; i<lengthOfVertice; i++) {
		Vec2 vertice = vertices[i];
		// CCLOG("vertice[%d]: %f, %f", i, vertice.x, vertice.y);
		auto debugLabel = Label::createWithTTF(__String::createWithFormat("%d", i)->getCString(), RESOURCES_FONT_COMIC_SANS, 24);
		debugLabel->setPosition(vertice);
		this->addChild(debugLabel, 1);
	}
}
#pragma endregion helper