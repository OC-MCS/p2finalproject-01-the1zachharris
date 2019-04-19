#pragma once

#include "Weapon.h"

#define SPRITE_COUNT_X 11
#define SPRITE_COUNT_Y 5
#define BLOCK_COUNT 4

using namespace sf;
using namespace std;

class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update(Time elapsedTime);
	void render();

	void InitSprites();
	void ResetSprites();

	void updateStatistics(Time elapsedTime);
	void HandleTexts();
	void HandleCollisionEnemyMasterWeaponPlayer();
	void HanldeEnemyMasterWeaponMoves();
	void HandleEnemyMasterWeaponFiring();
	void HandleCollisionEnemyMasterWeaponBlock();
	void HandleEnemyMasterMove();
	void HandleCollisionEnemyWeaponBlock();
	void HandleCollisionWeaponPlayer();
	void HanldeEnemyWeaponMoves();
	void HandleEnemyWeaponFiring();
	void HandleCollisionBlockEnemy();
	void HandleEnemyMoves();
	void HanldeWeaponMoves();
	void HandleCollisionWeaponBlock();
	void HandleCollisionWeaponEnemy();
	void HandleCollisionWeaponEnemyMaster();
	void HandleGameOver();
	void DisplayGameOver();
	void handlePlayerInput(Keyboard::Key key, bool isPressed);

private:
	static const float		PlayerSpeed;
	static const Time	TimePerFrame;

	RenderWindow		mWindow;
	Texture	mTexture;
	Sprite	mPlayer;
	Font	mFont;
	Text	mStatisticsText;
	Time	mStatisticsUpdateTime;
	Text	mText;
	Text	_LivesText;
	int _lives = 3;
	Text	_ScoreText;
	int _score = 0;

	size_t	mStatisticsNumFrames;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;

	bool _IsGameOver = false;
	bool _IsEnemyWeaponFired = false;
	bool _IsPlayerWeaponFired = false;
	bool _IsEnemyMasterWeaponFired = false;

	Texture	_TextureEnemy;
	Sprite	_Enemy[SPRITE_COUNT_X][SPRITE_COUNT_Y];
	Texture	_TextureBlock;
	Sprite	_Block[BLOCK_COUNT];
	Texture	_TextureWeapon;
	Texture	_TextureWeaponEnemy;
	Texture	_TextureWeaponEnemyMaster;
	Sprite	_Weapon;
	Texture	_TextureEnemyMaster;
	Sprite	_EnemyMaster;
};
