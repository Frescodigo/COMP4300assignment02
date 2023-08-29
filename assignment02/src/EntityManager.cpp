#include "EntityManager.h"


void EntityManager::update()
{
	for (auto e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_toAdd.clear();
	for (auto e : m_entities)
	{
		// if e is dead, remove from entities
		// if e is dead remove from entitymap
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::make_shared<Entity>(tag, m_totalEntities++);
	m_toAdd.push_back(e);
	return e;
}