#include "Ecs/ComponentArray.hpp"
#include "Ecs/Component.h"

using namespace PC_CORE;

ComponentArray::ComponentArray()
{
}

ComponentArray::ComponentArray(TypeId typeId) : m_ComponentType(typeId)
{
    const auto& type = ReflectorInstance().GetType(m_ComponentType);

    m_ComponentSize = static_cast<uint32_t>(type.size);
    constructor = type.metaData.createFunc;
    destructor = type.metaData.deleteFunc;
}

void ComponentArray::Add(EntityId entityId)
{
    if (m_EntityIndexData.size() <= entityId)
        m_EntityIndexData.resize(entityId + 1, std::numeric_limits<size_t>::max());

    if (m_EntityIndexData[entityId] == std::numeric_limits<size_t>::max())
    {
        // allocate new ellement
        size_t newIndex = m_Volume;

        m_EntityIndexData[entityId] = newIndex;

        if (m_ComponentData.size() < (newIndex + 1) * m_ComponentSize)
            m_ComponentData.resize((newIndex + 1) * m_ComponentSize);

        // set component Enitity Id
        *reinterpret_cast<uint32_t*>(&m_ComponentData[newIndex * m_ComponentSize]) = entityId;

        constructor(&m_ComponentData[newIndex * m_ComponentSize]);

        ++m_Volume;
    }
}


uint8_t& ComponentArray::Get(EntityId entityId)
{
    assert(entityId < m_EntityIndexData.size() && "OutSide range");
    assert(m_EntityIndexData[entityId] != std::numeric_limits<size_t>::max() && "Invalid index");


    return m_ComponentData[m_EntityIndexData[entityId] * m_ComponentSize];
}

const uint8_t& ComponentArray::Get(EntityId entityId) const
{
    assert(entityId < m_EntityIndexData.size() && "OutSide range");
    assert(m_EntityIndexData[entityId] != std::numeric_limits<size_t>::max() && "Invalid index");


    return m_ComponentData[m_EntityIndexData[entityId] * m_ComponentSize];
}

bool ComponentArray::HasComponent(EntityId entityId) const
{
    return entityId < m_EntityIndexData.size() &&
        m_EntityIndexData[entityId] != std::numeric_limits<size_t>::max();
}

void ComponentArray::Remove(EntityId entityId)
{
    if (!HasComponent(entityId))
    {
        PC_LOGERROR("Attempting to remove entity's {} non-existent component ({})", entityId, m_ComponentType);
        return;
    }

    size_t removedIndex = m_EntityIndexData[entityId];
    size_t lastIndex = m_Volume - 1;

    destructor(&m_ComponentData[removedIndex * m_ComponentSize]);

    if (removedIndex != lastIndex)
    {
        std::memcpy(&m_ComponentData[removedIndex * m_ComponentSize],
                    &m_ComponentData[lastIndex * m_ComponentSize],
                    m_ComponentSize);

        // update mapping
        for (auto& i : m_EntityIndexData)
        {
            if (i == lastIndex)
            {
                i = removedIndex;
                break;
            }
        }
    }

    m_EntityIndexData[entityId] = std::numeric_limits<size_t>::max();
    --m_Volume;
}
