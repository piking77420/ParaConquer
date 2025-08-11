#include "Ecs/EntityManager.h"

#include <Numbers>

#include "PerfRegion.hpp"
#include "Log.hpp"
#include "Ecs/Component.h"

using namespace PC_CORE;

EntityManager::EntityManager()
{
	PERF_REGION_SCOPED;
	DYNAMIC_REFLECT_INIT;
}


EntityId EntityManager::CreateEntity()
{
	PERF_REGION_SCOPED;

	assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
	
	EntityId id = INVALID_ENTITY_ID;
	if (m_AvailableEntitiesId.empty())
	{
		id = m_EntityIdCounter++;
	}
	else
	{
		id = m_AvailableEntitiesId.front();
		m_AvailableEntitiesId.pop();
	}

	assert(id != INVALID_ENTITY_ID);
	m_EntityEnableFlags.set(id,	true);

	// set name
	m_EntityNameAlloc.Add(id);
	char* namePtr = m_EntityNameAlloc.At(id).data();
	
	std::string name = "Entity_" + std::to_string(id);
	std::memcpy(namePtr, name.data(), name.size() + 1);
	
	// set signature
	m_EntitesSignature.Add(id);

	++m_LivingEntityCount;
	return id;
}

EntityId EntityManager::CreateEntity(const std::string& _name)
{
	PERF_REGION_SCOPED;

	assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
	assert(_name.size() < MAX_ENTITY_NAME_LENGHT && "Entity name is too long.");
	
	EntityId id = INVALID_ENTITY_ID;
	if (m_AvailableEntitiesId.empty())
	{
		id = m_EntityIdCounter++;
	}
	else
	{
		id = m_AvailableEntitiesId.front();
		m_AvailableEntitiesId.pop();
	}
	
	assert(id != INVALID_ENTITY_ID);
	m_EntityEnableFlags.set(id,	true);

	// set name
	m_EntityNameAlloc.Add(id);
	char* namePtr = &m_EntityNameAlloc.At(id)[0];
	std::memcpy(namePtr, _name.data(), _name.size() + 1);
	
	// set signature
	m_EntitesSignature.Add(id);

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

	const char* entityNameBegin = &m_EntityNameAlloc[_id][0];
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


	m_EntitesSignature.Remove(entityId);
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
	
	if (!m_EntitesSignature.Contain(entityId))
	{
		PC_LOGERROR("Attempting to SetSignature but m_EntitesSignature dense list failed for EntityId = {}.", entityId);
		return;
	}
	m_EntitesSignature[entityId] = signature;
}

Signature* EntityManager::GetSignature(EntityId entity)
{
	PERF_REGION_SCOPED;

	assert(entity < MAX_ENTITIES ,"Entity out of range.");

	if (!m_EntityEnableFlags.test(entity))
	{
		PC_LOGERROR("Attempting to GetSignature entity list that doesn't exist EntityId = {}.", entity);
		return nullptr;
	}

	if (!m_EntitesSignature.Contain(entity))
	{
		PC_LOGERROR("Attempting to GetSignature but m_EntitesSignature dense list failed for EntityId = {}.", entity);
		return nullptr;
		
	}

	return &m_EntitesSignature[entity];
}

const Signature* EntityManager::GetSignature(EntityId entity) const
{
	PERF_REGION_SCOPED;

	assert(entity < MAX_ENTITIES, "Entity out of range.");

	if (!m_EntityEnableFlags.test(entity))
	{
		PC_LOGERROR("Attempting to GetSignature entity list that doesn't exist EntityId = {}.", entity);
		return nullptr;
	}

	if (!m_EntitesSignature.Contain(entity))
	{
		PC_LOGERROR("Attempting to GetSignature but m_EntitesSignature dense list failed for EntityId = {}.", entity);
		return nullptr;
	}


	return &m_EntitesSignature[entity];
}
