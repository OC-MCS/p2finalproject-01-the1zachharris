#pragma once
#include "Entity.h"

using namespace std;

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

public:
	static vector<shared_ptr<Entity>> m_Entities;
	static shared_ptr<Entity> GetPlayer();
	static shared_ptr<Entity> GetEnemyMaster();
};