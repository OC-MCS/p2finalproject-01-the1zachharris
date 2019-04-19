#pragma once

using namespace sf;

enum EntityType
{
	player,
	weapon,
	enemyWeapon,
	enemyMasterWeapon,
	enemy,
	enemyMaster,
	block
};

class Entity
{
public:
	Entity() { };
	~Entity() { };

public:
	Sprite m_sprite;
	Vector2u m_size;
	Vector2f m_position;
	EntityType m_type;
	bool m_enabled = true;

	// Enemy only
	bool m_bLeftToRight = true;
	int m_times = 0;
};
