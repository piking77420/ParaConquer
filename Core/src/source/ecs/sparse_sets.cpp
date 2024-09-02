#include "ecs/sparse_sets.hpp"

#include <stdexcept>


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

void PC_CORE::SparseSet::Alloc(EntityId _entityId)
{
    if (m_SparseList.size() < _entityId)
    {
        throw std::invalid_argument("Sparse set is too small");
    }

    const size_t indexInDense = GetSize();
    m_SparseList.at(indexInDense) = _entityId;
    m_Dense.resize(indexInDense + GetDensity());
}

PC_CORE::SparseSet::SparseSet(size_t _densitySize) : m_Density(_densitySize)
{
    
}

PC_CORE::SparseSet::SparseSet(size_t _densitySize, size_t _size) : m_Density(_densitySize)
{
    m_Dense.resize(_size * _densitySize);
    m_SparseList.resize(_size);
}

PC_CORE::SparseSet::SparseSet(size_t _densitySize, size_t _size, CreateFunc _createFunc, DeleteFunc _deleteFunc) : SparseSet(_densitySize,_size)
{
    m_CreateFunc = _createFunc;
    m_DeleteFunc = _deleteFunc;
}
