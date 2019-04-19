#include "pch.h"
#include "EntityManager.h"

using namespace std;

vector<shared_ptr<Entity>> EntityManager::m_Entities;

EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
}

shared_ptr<Entity> EntityManager::GetPlayer()
{
	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type == EntityType::player)
		{
			return entity;
		}
	}

	return nullptr;
}

shared_ptr<Entity> EntityManager::GetEnemyMaster()
{
	for (shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_type == EntityType::enemyMaster)
		{
			return entity;
		}
	}

	return nullptr;
}