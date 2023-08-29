#include "EntityManager.h"

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
	for (std::shared_ptr<Entity> entity : m_toAdd)
	{
		m_entities.push_back(entity);
	}
	m_toAdd.clear();
}

void EntityManager::removeDeadEntities()
{
	// TODO
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_toAdd.push_back(e);
	return e;
}

EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

//EntityVec& EntityManager::getEntities(const std::string& tag)
//{
//	//TODO
//	return m_entities;
//}

