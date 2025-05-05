#include "ecs/entity_manager.h"

#include <numbers>

#include "log.hpp"
#include "ecs/component.h"

using namespace PC_CORE;

EntityManager::EntityManager()
{
	m_Entities.resize(MAX_ENTITIES);
	m_EntitesSignature.resize(MAX_ENTITIES);

	for (EntityId entity = 0; entity < MAX_ENTITIES; ++entity)
	{
		m_AvailableEntitiesId.push(entity);
	}
	
}

EntityManager::~EntityManager()
{
}

EntityId EntityManager::CreateEntity()
{
	assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

	EntityId id = m_AvailableEntitiesId.front();
	m_AvailableEntitiesId.pop();
	++m_LivingEntityCount;

	return id;
}

void EntityManager::RemoveEntity(EntityId entityId)
{
	assert(entityId < MAX_ENTITIES && "Entity out of range.");


	m_EntitesSignature[entityId].reset();

	// push the new available id
	m_AvailableEntitiesId.push(entityId);

	--m_LivingEntityCount;

}

void EntityManager::SetSignature(EntityId entityId, const Signature& signature)
{
	m_EntitesSignature[entityId] = signature;
}

Signature& EntityManager::GetSignature(EntityId entity)
{
	assert(entity < MAX_ENTITIES ,"Entity out of range.");

	return m_EntitesSignature[entity];
}

const Signature& EntityManager::GetSignature(EntityId entity) const
{
	assert(entity < MAX_ENTITIES, "Entity out of range.");

	return m_EntitesSignature[entity];
}
