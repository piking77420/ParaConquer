#include "data_structures/type_sparse_set.hpp"

using namespace PC_CORE;

TypeSparseSet::TypeSparseSet(const PC_CORE::ReflectedType& _type) : m_SparseSet(_type.typeSize), m_Type(&_type)
{
}

TypeSparseSet::~TypeSparseSet()
{
    // TO DO DESTROY
}

uint8_t* TypeSparseSet::Create(size_t _index)
{
    uint8_t* element =  m_SparseSet.Alloc(_index);

    if (element == nullptr)
        return nullptr;

    if (m_Type->createFunc)
        m_Type->createFunc(element);
    
    return element;
}

void TypeSparseSet::Free(size_t _index)
{
    uint8_t* element = &m_SparseSet[_index];

    if (m_Type->deleteFunc)
        m_Type->deleteFunc(element);

    m_SparseSet.Free(_index);
}

bool TypeSparseSet::IsValid(size_t _index) const
{
    const uint8_t* element = &m_SparseSet[_index];

    return element != nullptr;
}
