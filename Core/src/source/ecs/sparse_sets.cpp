#include "ecs/sparse_sets.hpp"

#include <stdexcept>

#include "log.hpp"


uint8_t* PC_CORE::SparseSet::GetData()
{
    return m_Dense.data();
}

const uint8_t* PC_CORE::SparseSet::GetData() const
{
    return m_Dense.data();
}

size_t PC_CORE::SparseSet::GetSize() const
{
    return m_Dense.size() / m_Density;
}

size_t PC_CORE::SparseSet::GetDensity() const
{
    return m_Density;
}

uint8_t* PC_CORE::SparseSet::Alloc(EntityId _entityId)
{
    if (m_SparseList.at(_entityId) != NULL_INDEX)
    {
        PC_LOGERROR("This entity Has already have index")
        return nullptr;
    }
    const size_t denseSize = m_Dense.size();
    m_SparseList.at(_entityId) = static_cast<uint32_t>(m_Dense.size());
    m_Dense.resize(denseSize + m_Density);
    return m_Dense.data() + denseSize;
}

void PC_CORE::SparseSet::Free(EntityId _entityId)
{
    if (Empty())
        return;
    
    const size_t index = m_SparseList.at(_entityId);
    if (index == NULL_INDEX)
    {
       return;
    }

    if (index == 0)
    {
        m_Dense.pop_back();
        m_SparseList.at(_entityId) = NULL_INDEX;
        return;
    }

    // Find Last entity who has the last dense element
    size_t lastEntity = -1;
    size_t EntitylastIndex = 0;
    for (size_t ent = 0; ent < m_SparseList.size(); ent++)
    {
        if (m_SparseList.at(ent) != NULL_INDEX && EntitylastIndex < m_SparseList.at(ent))
        {
            EntitylastIndex = m_SparseList.at(ent);
            lastEntity = ent;
        }
    }

    std::swap(m_Dense.at(index), m_Dense.at(EntitylastIndex));
    m_SparseList.at(lastEntity) = static_cast<uint32_t>(index);
    m_SparseList.at(_entityId) = NULL_INDEX;
    m_Dense.pop_back();
}

const uint8_t* PC_CORE::SparseSet::GetEntityData(EntityId entity_id) const
{
    if (m_SparseList.at(entity_id) == NULL_INDEX)
        return nullptr;

    return &m_Dense.at(m_SparseList.at(entity_id));
}

uint8_t* PC_CORE::SparseSet::GetEntityData(EntityId entity_id)
{
    if (m_SparseList.at(entity_id) == NULL_INDEX)
        return nullptr;

    return &m_Dense.at(m_SparseList.at(entity_id));
}

bool PC_CORE::SparseSet::Empty() const
{
    return m_Dense.empty();
}

PC_CORE::SparseSet::SparseSet(size_t _densitySize) : m_Density(_densitySize)
{
}

PC_CORE::SparseSet::SparseSet(size_t _densitySize, size_t _size) : m_Density(_densitySize)
{
    m_SparseList.resize(_size);
    for (size_t i = 0; i < _size; i++)
    {
        m_SparseList.at(i) = NULL_INDEX;
    }
}

