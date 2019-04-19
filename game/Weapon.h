#pragma once

using namespace sf;

class Weapon
{
public:
	Weapon();
	~Weapon();

public:
	Texture	_texture;
	Sprite	_sprite;
	bool		_enabled = false;
};