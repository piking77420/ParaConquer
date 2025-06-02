#include "ecs/entity_manager.h"

#include <numbers>

#include "perf_region.hpp"
#include "log.hpp"
#include "ecs/component.h"

using namespace PC_CORE;

EntityManager::EntityManager()
{
	PERF_REGION_SCOPED;

	m_EntitesSignature.resize(MAX_ENTITIES);
	m_EntityNameAlloc.resize(MAX_ENTITIES * MAX_ENTITY_NAME_LENGHT);

	for (EntityId entity = 0; entity < MAX_ENTITIES; entity++)
	{
		m_AvailableEntitiesId.push(entity);
	}
	
}

EntityManager::~EntityManager()
{
}

EntityId EntityManager::CreateEntity(const std::string& _name)
{
	PERF_REGION_SCOPED;

	assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
	assert(_name.size() < MAX_ENTITY_NAME_LENGHT && "Entity name is too long.");
	
	EntityId id = m_AvailableEntitiesId.front();
	m_EntityEnableFlags.set(id,	true);
	m_AvailableEntitiesId.pop();
	std::memcpy(&m_EntityNameAlloc[id * MAX_ENTITY_NAME_LENGHT],_name.data(), _name.size() + 1);
	
	++m_LivingEntityCount;

	
	return id;
}

EntityId EntityManager::CreateEntity(std::string&& _name)
{
	PERF_REGION_SCOPED;

	assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
	assert(_name.size() < MAX_ENTITY_NAME_LENGHT && "Entity name is too long.");
	
	EntityId id = m_AvailableEntitiesId.front();
	m_EntityEnableFlags.set(id,	true);
	m_AvailableEntitiesId.pop();
	*reinterpret_cast<std::string*>(&m_EntityNameAlloc[id * MAX_ENTITY_NAME_LENGHT]) = std::move(_name);
	++m_LivingEntityCount;

	return id;
}

std::string_view EntityManager::GetEntityName(EntityId _id) const
{
	PERF_REGION_SCOPED;

	if (!m_EntityEnableFlags.test(false))
	{
		PC_LOGERROR("Attempting to get entity's name from entity list that doesn't exist EntityId = {}.", _id);
		return {};
	}

	const char* entityNameBegin = &m_EntityNameAlloc[_id * MAX_ENTITY_NAME_LENGHT];
	size_t lenght = std::strlen(entityNameBegin);
	
	return std::string_view(entityNameBegin, lenght);
}

void EntityManager::RemoveEntity(EntityId entityId)
{
	PERF_REGION_SCOPED;

	assert(entityId < MAX_ENTITIES && "Entity out of range.");

	if (!m_EntityEnableFlags.test(entityId))
	{
		PC_LOGERROR("Attempting to remove entity from entity list that doesn't exist EntityId = {}.", entityId);
		return;
	}


	m_EntitesSignature[entityId].reset();
	m_EntityEnableFlags.set(entityId,	false);

	// push the new available id
	m_AvailableEntitiesId.push(entityId);

	--m_LivingEntityCount;

}

void EntityManager::SetSignature(EntityId entityId, const Signature& signature)
{
	PERF_REGION_SCOPED;

	if (!m_EntityEnableFlags.test(entityId))
	{
		PC_LOGERROR("Attempting to SetSignature entity list that doesn't exist EntityId = {}.", entityId);
		return;
	}
	
	m_EntitesSignature[entityId] = signature;
}

Signature& EntityManager::GetSignature(EntityId entity)
{
	PERF_REGION_SCOPED;

	assert(entity < MAX_ENTITIES ,"Entity out of range.");

	if (!m_EntityEnableFlags.test(entity))
	{
		PC_LOGERROR("Attempting to GetSignature entity list that doesn't exist EntityId = {}.", entity);
	}

	return m_EntitesSignature[entity];
}

const Signature& EntityManager::GetSignature(EntityId entity) const
{
	PERF_REGION_SCOPED;

	assert(entity < MAX_ENTITIES, "Entity out of range.");

	if (!m_EntityEnableFlags.test(entity))
	{
		PC_LOGERROR("Attempting to GetSignature entity list that doesn't exist EntityId = {}.", entity);
	}


	return m_EntitesSignature[entity];
}
