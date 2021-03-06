
#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

using namespace sf;
using namespace std;

const float Game::PlayerSpeed = 100.f;
const Time Game::TimePerFrame = seconds(1.f / 60.f);

Game::Game()
	: mWindow(VideoMode(840, 600), "Space Invaders", Style::Close)
	, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);

	_TextureWeapon.loadFromFile("Media/Textures/SI_WeaponGreen.png");
	_TextureWeaponEnemy.loadFromFile("Media/Textures/SI_WeaponYellow.png");
	_TextureWeaponEnemyMaster.loadFromFile("Media/Textures/SI_WeaponRed.png");
	mTexture.loadFromFile("Media/Textures/SI_Player.png");
	_TextureEnemyMaster.loadFromFile("Media/Textures/SI_EnemyMaster.png");
	_TextureEnemy.loadFromFile("Media/Textures/SI_Enemy.png");
	_TextureBlock.loadFromFile("Media/Textures/SI_Block.png");
	mFont.loadFromFile("Media/Sansation.ttf");

	InitSprites();
}

void Game::ResetSprites()
{
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsEnemyMasterWeaponFired = false;

	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		entity->m_enabled = true;
	}
}

void Game::InitSprites()
{
	_lives = 3;
	_score = 0;
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsEnemyMasterWeaponFired = false;

	//
	// Player
	//

	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(100.f, 500.f);
	shared_ptr<Entity> player = make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);

	//
	// Enemy Master
	//

	_EnemyMaster.setTexture(_TextureEnemyMaster);
	_EnemyMaster.setPosition(100.f + 50.f, 1.f);
	shared_ptr<Entity> sem = make_shared<Entity>();
	sem->m_sprite = _EnemyMaster;
	sem->m_type = EntityType::enemyMaster;
	sem->m_size = _TextureEnemyMaster.getSize();
	sem->m_position = _EnemyMaster.getPosition();
	EntityManager::m_Entities.push_back(sem);

	//
	// Enemies
	//

	for (int i = 0; i < SPRITE_COUNT_X; i++)
	{
		for (int j = 0; j < SPRITE_COUNT_Y; j++)
		{
			_Enemy[i][j].setTexture(_TextureEnemy);
			_Enemy[i][j].setPosition(100.f + 50.f * (i + 1), 10.f + 50.f * (j + 1));

			shared_ptr<Entity> se = make_shared<Entity>();
			se->m_sprite = _Enemy[i][j];
			se->m_type = EntityType::enemy;
			se->m_size = _TextureEnemy.getSize();
			se->m_position = _Enemy[i][j].getPosition();
			EntityManager::m_Entities.push_back(se);
		}
	}

	//
	// Blocks
	//

	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		_Block[i].setTexture(_TextureBlock);
		_Block[i].setPosition(0.f + 150.f * (i + 1), 10 + 350.f);

		shared_ptr<Entity> sb = make_shared<Entity>();
		sb->m_sprite = _Block[i];
		sb->m_type = EntityType::block;
		sb->m_size = _TextureBlock.getSize();
		sb->m_position = _Block[i].getPosition();
		EntityManager::m_Entities.push_back(sb);
	}

	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);

	//
	// Lives
	//

	_LivesText.setFillColor(Color::Green);
	_LivesText.setFont(mFont);
	_LivesText.setPosition(10.f, 50.f);
	_LivesText.setCharacterSize(20);
	_LivesText.setString(to_string(_lives));

	//
	// Text
	//

	_ScoreText.setFillColor(Color::Green);
	_ScoreText.setFont(mFont);
	_ScoreText.setPosition(10.f, 100.f);
	_ScoreText.setCharacterSize(20);
	_ScoreText.setString(to_string(_score));
}

void Game::run()
{
	Clock clock;
	Time timeSinceLastUpdate = Time::Zero;
	while (mWindow.isOpen())
	{
		Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(Time elapsedTime)
{
	Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::player)
		{
			continue;
		}

		entity->m_sprite.move(movement * elapsedTime.asSeconds());
	}
}

void Game::render()
{
	mWindow.clear();

	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	mWindow.draw(mStatisticsText);
	mWindow.draw(mText);
	mWindow.draw(_LivesText);
	mWindow.draw(_ScoreText);
	mWindow.display();
}

void Game::updateStatistics(Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	//
	// Handle collisions
	//

	if (mStatisticsUpdateTime >= seconds(0.050f))
	{
		if (_IsGameOver == true)
			return;

		HandleTexts();
		HandleGameOver();
		HandleCollisionWeaponEnemy();
		HandleCollisionWeaponPlayer();
		HandleCollisionWeaponBlock();
		HandleCollisionEnemyWeaponBlock();
		HandleCollisionEnemyMasterWeaponBlock();
		HandleCollisionEnemyMasterWeaponPlayer();
		HandleCollisionBlockEnemy();
		HandleCollisionWeaponEnemyMaster();
		HanldeWeaponMoves();
		HanldeEnemyWeaponMoves();
		HanldeEnemyMasterWeaponMoves();
		HandleEnemyMoves();
		HandleEnemyMasterMove();
		HandleEnemyWeaponFiring();
		HandleEnemyMasterWeaponFiring();
	}
}

void Game::HandleTexts()
{
	string lives = "Lives: " + to_string(_lives);
	_LivesText.setString(lives);
	string score = "Score: " + to_string(_score);
	_ScoreText.setString(score);
	return;
}

void Game::HandleCollisionEnemyMasterWeaponPlayer()
{
	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyMasterWeaponFired = false;
			_lives--;
			goto end;
		}
	}

end:
	return;
}

void Game::HanldeEnemyMasterWeaponMoves()
{
	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y++;

		if (y >= 600)
		{
			entity->m_enabled = false;
			_IsEnemyMasterWeaponFired = false;
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleEnemyMasterWeaponFiring()
{
	if (_IsEnemyMasterWeaponFired == true)
		return;

	if (EntityManager::GetEnemyMaster()->m_enabled == false)
		return;

	int r = rand() % 50;
	if (r != 10)
		return;

	float x, y;
	x = EntityManager::GetEnemyMaster()->m_sprite.getPosition().x;
	y = EntityManager::GetEnemyMaster()->m_sprite.getPosition().y;
	y--;

	shared_ptr<Entity> sw = make_shared<Entity>();
	sw->m_sprite.setTexture(_TextureWeaponEnemyMaster);

	sw->m_sprite.setPosition(
		x + _TextureEnemyMaster.getSize().x / 2,
		y + _TextureEnemyMaster.getSize().y);
	sw->m_type = EntityType::enemyMasterWeapon;
	sw->m_size = _TextureWeaponEnemyMaster.getSize();
	EntityManager::m_Entities.push_back(sw);

	_IsEnemyMasterWeaponFired = true;
}

void Game::HandleCollisionEnemyMasterWeaponBlock()
{
	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		for (shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundBlock) == true)
			{
				weapon->m_enabled = false;
				_IsEnemyMasterWeaponFired = false;
				goto end2;
			}
		}
	}

end2:
	return;
}

void Game::HandleEnemyMasterMove()
{
	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyMaster)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

		if (entity->m_bLeftToRight == true)
			x = x + 0.5f;
		else
			x = x - 0.5f;

		entity->m_times++;

		if (x >= ((BLOCK_COUNT) * 150) || x <= 150)
		{
			if (entity->m_bLeftToRight == true)
			{
				entity->m_bLeftToRight = false;
				entity->m_times = 0;
			}
			else
			{
				entity->m_bLeftToRight = true;
				entity->m_times = 0;
			}
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleCollisionEnemyWeaponBlock()
{
	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		for (shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundBlock) == true)
			{
				weapon->m_enabled = false;
				_IsEnemyWeaponFired = false;
				goto end2;
			}
		}
	}

end2:
	return;
}

void Game::HandleCollisionWeaponPlayer()
{
	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyWeaponFired = false;
			_lives--;
			goto end;
		}
	}

end:
	return;
}

void Game::HanldeEnemyWeaponMoves()
{
	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y += 1.0f;

		if (y >= 600)
		{
			entity->m_enabled = false;
			_IsEnemyWeaponFired = false;
		}
		else
		{
			entity->m_sprite.setPosition(x, y);
		}
	}
}

void Game::HandleEnemyWeaponFiring()
{
	if (_IsEnemyWeaponFired == true)
		return;

	vector<shared_ptr<Entity>>::reverse_iterator rit = EntityManager::m_Entities.rbegin();
	for (; rit != EntityManager::m_Entities.rend(); rit++)
	{
		shared_ptr<Entity> entity = *rit;

		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		int r = rand() % 20;
		if (r != 10)
			continue;

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y--;

		shared_ptr<Entity> sw = make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeaponEnemy);
		sw->m_sprite.setPosition(
			x + _TextureWeaponEnemy.getSize().x / 2,
			y + _TextureWeaponEnemy.getSize().y);

		sw->m_sprite.setPosition(
			entity->m_sprite.getPosition().x + _TextureEnemy.getSize().x / 2,
			entity->m_sprite.getPosition().y - 10);

		sw->m_type = EntityType::enemyWeapon;
		sw->m_size = _TextureWeaponEnemy.getSize();
		EntityManager::m_Entities.push_back(sw);

		_IsEnemyWeaponFired = true;
		break;
	}
}

void Game::HandleCollisionBlockEnemy()
{
	// Handle collision ennemy blocks

	for (shared_ptr<Entity> enemy : EntityManager::m_Entities)
	{
		if (enemy->m_enabled == false)
		{
			continue;
		}

		if (enemy->m_type != EntityType::enemy)
		{
			continue;
		}

		for (shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundEnemy.intersects(boundBlock) == true)
			{
				EntityManager::GetPlayer()->m_enabled = false;
				goto end3;
			}
		}
	}

end3:
	return;
}

void Game::HandleEnemyMoves()
{
	//
	// Handle Enemy moves
	//

	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

		if (entity->m_bLeftToRight == true)
			x++;
		else
			x--;
		entity->m_times++;

		if (entity->m_times >= 100) 
		{
			if (entity->m_bLeftToRight == true)
			{
				entity->m_bLeftToRight = false;
				entity->m_times = 0;
			}
			else
			{
				entity->m_bLeftToRight = true;
				entity->m_times = 0;
				y += 1;
			}
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HanldeWeaponMoves()
{
	//
	// Handle Weapon moves
	//

	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::weapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y--;

		if (y <= 0)
		{
			entity->m_enabled = false;
			_IsPlayerWeaponFired = false;
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleCollisionWeaponBlock()
{
	// Handle collision weapon blocks

	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundBlock) == true)
			{
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				goto end2;
			}
		}
	}

end2:
	return;
}

void Game::HandleCollisionWeaponEnemy()
{
	// Handle collision weapon enemies

	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type != EntityType::enemy)
			{
				continue;
			}

			if (enemy->m_enabled == false)
			{
				continue;
			}

			FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundEnemy) == true)
			{
				enemy->m_enabled = false;
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_score += 10;
				goto end;
			}
		}
	}

end:
	return;
}

void Game::HandleCollisionWeaponEnemyMaster()
{
	// Handle collision weapon master enemy

	for (shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type != EntityType::enemyMaster)
			{
				continue;
			}

			if (enemy->m_enabled == false)
			{
				continue;
			}

			FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundEnemy) == true)
			{
				enemy->m_enabled = false;
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_score += 100;
				goto end;
			}
		}
	}

end:
	return;
}

void Game::HandleGameOver()
{
	// Game Over ?
	int count = count_if(EntityManager::m_Entities.begin(), EntityManager::m_Entities.end(), [](shared_ptr<Entity> element) {
		if (element->m_type == EntityType::enemy || element->m_type == EntityType::enemyMaster)
		{
			if (element->m_enabled == false)
			{
				return true;
			}
		}
		return false;
	});

	// sprite counts + enemy master
	//if (count >= (5))
	if (count == ((SPRITE_COUNT_X * SPRITE_COUNT_Y) + 1))
	{
		DisplayGameOver();
	}

	if (EntityManager::GetPlayer()->m_enabled == false)
	{
		DisplayGameOver();
	}

	if (_lives == 0)
	{
		DisplayGameOver();
	}
}

void Game::DisplayGameOver()
{
	if (_lives == 0)
	{
		mText.setFillColor(Color::Green);
		mText.setFont(mFont);
		mText.setPosition(200.f, 200.f);
		mText.setCharacterSize(80);

		mText.setString("GAME OVER");

		_IsGameOver = true;
	}
	else
	{
		ResetSprites();
	}
}

void Game::handlePlayerInput(Keyboard::Key key, bool isPressed)
{
	if (key == Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == Keyboard::Right)
		mIsMovingRight = isPressed;

	if (key == Keyboard::Space)
	{
		if (isPressed == false)
		{
			return;
		}

		if (_IsPlayerWeaponFired == true)
		{
			return;
		}

		shared_ptr<Entity> sw = make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeapon);
		sw->m_sprite.setPosition(
			EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2,
			EntityManager::GetPlayer()->m_sprite.getPosition().y - 10);
		sw->m_type = EntityType::weapon;
		sw->m_size = _TextureWeapon.getSize();
		EntityManager::m_Entities.push_back(sw);

		_IsPlayerWeaponFired = true;
	}
}