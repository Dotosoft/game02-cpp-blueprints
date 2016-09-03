#include "spacecraze\Player.h"
#include "spacecraze\GameWorld.h"

using namespace spacecraze;

Player::Player(GameWorld* game_world)
{
	game_world_ = game_world;
	size_ = Size(50, 50);
	lives_ = 0;
	is_respawning_ = false;
}

Player::~Player()
{}

Player* Player::createWithSpriteFrameName(GameWorld* game_world, const char* frame_name)
{
	auto player = new Player(game_world);
	if(player && player->initWithSpriteFrameName(frame_name))
	{
		player->autorelease();
		return player;
	}
	CC_SAFE_DELETE(player);
	return NULL;
}

void Player::Enter()
{
	// initially position the player below the screen
	setPosition( Vec2(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * -0.1) );

	// animate the move into the screen
	auto movement = EaseBackOut::create(MoveTo::create(1.0f, Vec2(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * 0.1)));
	auto movement_over = CallFunc::create(this, callfunc_selector(Player::EnterFinished));
	runAction(Sequence::createWithTwoActions(movement, movement_over));
}

void Player::EnterFinished()
{
	// player has entered, now start the game
	game_world_->StartGame();
}

void Player::Leave()
{
	// no enemies left, fly out the top edge of the screen
	auto movement = EaseBackIn::create(MoveBy::create(1.0f, Vec2(0, SCREEN_SIZE.height)));
	runAction(movement);
}

void Player::Die()
{
	// first reduce lives
	lives_ = (--lives_ < 0) ? 0 : lives_;

	// respawn only if there are lives remaining
	is_respawning_ = (lives_ > 0);

	// animate the death :(
	auto death = Spawn::createWithTwoActions(FadeOut::create(0.5f), ScaleTo::create(0.5f, 1.5f));
	// call the appropriate callback based on lives remaining
	auto after_death = (lives_ <= 0) ? (CallFunc::create(this, callfunc_selector(Player::OnAllLivesFinished))) : (CallFunc::create(this, callfunc_selector(Player::Respawn)));
	runAction(Sequence::createWithTwoActions(death, after_death));

	// play a particle...a sad one :(
	auto explosion = ParticleSystemQuad::create(RESOURCES_SPAZECRAZE_SHEET_EXPLOTION);
	explosion->setAutoRemoveOnFinish(true);
	// explosion->setPosition(m_obPosition);
	explosion->setPosition(getPosition());
	game_world_->addChild(explosion);

	SOUND_ENGINE->playEffect(RESOURCES_SPAZECRAZE_SOUND_BLAST_PLAYER);
}

void Player::Respawn()
{
	// reset the position, opacity & scale
	setPosition(Vec2(SCREEN_SIZE.width * 0.5, SCREEN_SIZE.height * 0.1));
	setOpacity(255);
	setScale(0.0f);

	// animate the respawn
	auto respawn = Spawn::createWithTwoActions(ScaleTo::create(0.5f, 1.0f), Blink::create(1.0f, 5));
	auto respawn_complete = CallFunc::create(this, callfunc_selector(Player::OnRespawnComplete));
	runAction(Sequence::createWithTwoActions(respawn, respawn_complete));
}

void Player::OnRespawnComplete()
{
	is_respawning_ = false;
}

void Player::OnAllLivesFinished()
{
	// player is finally dead...for sure...game is now over
	game_world_->GameOver();
}
