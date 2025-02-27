
#include "ecs/component_array.hpp"

using namespace PC_CORE;

ComponentArray::ComponentArray()
{

}

ComponentArray::ComponentArray(const ComponentArrayCreateInfo& componentArrayCreateInfo) : m_ComponentArrayInfo(componentArrayCreateInfo)
{
	assert(componentArrayCreateInfo.componentSize != 0 && componentArrayCreateInfo.constructor != nullptr &&
		componentArrayCreateInfo.destructor != nullptr, "Invalid Parameters");

	m_ComponentData.resize(MAX_ENTITIES * componentArrayCreateInfo.componentSize);
}

void ComponentArray::AddEntityData(EntityId entityId)
{
	if (m_EntityToIndex.find(entityId) == m_EntityToIndex.end())
	{
		PushData(entityId);
	}
}

void ComponentArray::PushData(EntityId entityId)
{
	assert(m_EntityToIndex.find(entityId) == m_EntityToIndex.end(), "This entity has aldrealy this component");

	// Push New Component
	const size_t newIndex = m_Volume * m_ComponentArrayInfo.componentSize;
	m_EntityToIndex[entityId] = newIndex;
	m_IndexToEntity[newIndex] = entityId;
	m_ComponentArrayInfo.constructor(&m_ComponentData[newIndex]);
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
	const size_t indexOfLastElement = (m_Volume - 1) * m_ComponentArrayInfo.componentSize;

	m_ComponentArrayInfo.destructor(&m_ComponentData[indexOfRemovedEntity]);
	memcpy(&m_ComponentData[indexOfRemovedEntity], &m_ComponentData[indexOfRemovedEntity], m_ComponentArrayInfo.componentSize);

	EntityId entityOfLastElement = m_IndexToEntity[indexOfLastElement];
	m_EntityToIndex[entityOfLastElement] = indexOfRemovedEntity;
	m_IndexToEntity[indexOfRemovedEntity] = entityOfLastElement;

	m_IndexToEntity.erase(entityId);
	m_IndexToEntity.erase(indexOfLastElement);

	--m_Volume;
}
