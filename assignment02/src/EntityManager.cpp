#include "EntityManager.h"

#include <iostream>

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
	for (std::shared_ptr<Entity> entity : m_toAdd)
	{
		m_entities.push_back(entity);
		m_entityMap[entity->tag()].push_back(entity);
	}
	m_toAdd.clear();

	// remove dead entities from entity vector
	removeDeadEntities(m_entities);

	// remove  dead entities from each vector in the map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}

}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// TODO
	auto result = std::remove_if(vec.begin(), vec.end(), [&vec](std::shared_ptr<Entity> entity) {
		return !entity->isActive();
	});

	vec.erase(result, vec.end());
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

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	//TODO
	// find returns an iterator, but it probably contatins (key, value)
	// so that's why second
	return m_entityMap[tag];
}

