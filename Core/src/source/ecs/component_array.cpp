
#include "ecs/component_array.hpp"
#include "ecs/component.h"

using namespace PC_CORE;

ComponentArray::ComponentArray()
{
	try
	{
		GetComponentInfo();

	}
	catch (...)
	{

	}
}

ComponentArray::ComponentArray(TypeId typeId) : componentType(typeId)
{
	GetComponentInfo();
	m_ComponentData.reserve(MAX_ENTITIES * componentSize);
}

void ComponentArray::AddEntityData(EntityId entityId)
{
	assert(m_EntityToIndex.find(entityId) == m_EntityToIndex.end(), "This entity has aldrealy this component");

	if (m_EntityToIndex.find(entityId) == m_EntityToIndex.end())
	{
		PushData(entityId);
	}
}

void ComponentArray::PushData(EntityId entityId)
{

	// Push New Component
	const size_t newIndex = m_Volume * componentSize;
	m_EntityToIndex[entityId] = newIndex;
	m_IndexToEntity[newIndex] = entityId;
	
	if (newIndex >= m_ComponentData.size())
	{
		m_ComponentData.resize(m_ComponentData.size() + componentSize);
		Component* c = reinterpret_cast<Component*>(&m_ComponentData[newIndex]);

	}
	else
	{
		// Scary undefined behaviours 
		Component* c = reinterpret_cast<Component*>(&m_ComponentData[newIndex]);
		uint32_t& ComponentEntityId = *reinterpret_cast<uint32_t*>(c);
		ComponentEntityId = entityId;
	}

	

	

	constructor(&m_ComponentData[newIndex]);
	m_Volume++;
}

uint8_t& ComponentArray::GetData(EntityId entityId)
{
	assert(m_EntityToIndex.find(entityId) != m_EntityToIndex.end() && "Get non-existent component.");

	return m_ComponentData[m_EntityToIndex[entityId]];
}

const uint8_t& ComponentArray::GetData(EntityId entityId) const
{
	assert(m_EntityToIndex.find(entityId) != m_EntityToIndex.end() && "Get non-existent component.");

	return m_ComponentData[m_EntityToIndex.at(entityId)];
}

void ComponentArray::RemoveEntityData(EntityId entityId)
{
	if (m_EntityToIndex.find(entityId) != m_EntityToIndex.end())
	{
		RemoveData(entityId);
	}

}

bool ComponentArray::HasComponent(EntityId entityId) const
{
	return m_EntityToIndex.find(entityId) != m_EntityToIndex.end();
}

void ComponentArray::RemoveData(EntityId entityId)
{
	assert(m_EntityToIndex.find(entityId) != m_EntityToIndex.end() && "Removing non-existent component");

	if (m_Volume == 0)
		return;

	const size_t indexOfRemovedEntity = m_EntityToIndex[entityId];
	const size_t indexOfLastElement = (m_Volume - 1) * componentSize;

	destructor(&m_ComponentData[indexOfRemovedEntity]);
	memcpy(&m_ComponentData[indexOfRemovedEntity], &m_ComponentData[indexOfRemovedEntity], componentSize);

	EntityId entityOfLastElement = m_IndexToEntity[indexOfLastElement];
	m_EntityToIndex[entityOfLastElement] = indexOfRemovedEntity;
	m_IndexToEntity[indexOfRemovedEntity] = entityOfLastElement;

	m_IndexToEntity.erase(entityId);
	m_IndexToEntity.erase(indexOfLastElement);

	--m_Volume;
}

void ComponentArray::GetComponentInfo()
{
	const auto& type = Reflector::GetType(componentType);


	componentSize = type.size;
	constructor = type.metaData.createFunc;
	destructor = type.metaData.deleteFunc;

}
