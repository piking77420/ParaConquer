#pragma once


#include "reflection/reflector.hpp"
#include "sparse_sets.hpp"

BEGIN_PCCORE

using TypeCreateFunc = void (*)(void*);
using TypeDeleteFunc = void (*)(void*);



class   TypeSparseSet
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(TypeSparseSet)

    PC_CORE_API TypeSparseSet(const PC_CORE::ReflectedType& _type);

    PC_CORE_API TypeSparseSet() = default;
    
    PC_CORE_API ~TypeSparseSet();

    template<typename T>
    T* begin()
    {
        return reinterpret_cast<T*>(m_SparseSet.GetData());	
    }

    template<typename T>
    const T* end() const
    {
        return m_SparseSet.end<T>();
    }

    template<typename T>
    uint8_t& operator[](size_t _index)
    {
        return m_SparseSet[_index];
    }
    
    const uint8_t& operator[](size_t _index) const
    {
        return m_SparseSet[_index];
    }

    PC_CORE_API uint8_t* Create(size_t _index);

    PC_CORE_API void Free(size_t _index);

    PC_CORE_API  bool IsValid(size_t _index) const;

private:
    PC_CORE::SparseSet m_SparseSet;

    const PC_CORE::ReflectedType* m_Type = nullptr;
};

END_PCCORE