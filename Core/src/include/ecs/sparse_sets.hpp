#pragma once
#include <vector>

#include "core_header.hpp"
#include "ecs/ecs_header.h"

BEGIN_PCCORE

using SparsetCreateFunc = void (*)(void*);
using SparsetDeleteFunc = void (*)(void*);

class SparseSet
{
public:

	DEFAULT_COPY_MOVE_OPERATIONS(SparseSet)

	PC_CORE_API std::vector<uint8_t>* GetData();

	PC_CORE_API const std::vector<uint8_t>* GetData() const;
	
	PC_CORE_API size_t GetSize() const;

	PC_CORE_API size_t GetDensity() const;

	PC_CORE_API uint8_t* Alloc(EntityId entity_id);

	PC_CORE_API void Free(EntityId entity_id);

	PC_CORE_API const uint8_t* GetEntityData(EntityId entity_id) const;

	PC_CORE_API uint8_t* GetEntityData(EntityId entity_id);

	PC_CORE_API bool Empty() const;

	PC_CORE_API SparseSet() = default;

	PC_CORE_API ~SparseSet() = default;
	
	PC_CORE_API SparseSet(size_t _densitySize);

	PC_CORE_API SparseSet(size_t _densitySize, size_t _size , SparsetCreateFunc _sparsetCreateFunc
		,SparsetDeleteFunc _sparsetDeleteFunc);
	
	template<typename T>
	T* begin()
	{
		return reinterpret_cast<T*>(GetData());	
	}

	template<typename T>
	const T* end() const
	{
		return GetData() + m_Dense.size();
	}
	
	template<typename T>
	T& operator[](size_t _index)
	{
		const size_t indexInSparse = m_SparseList.at(_index);
		return m_Dense.at(indexInSparse);
	}
	template<typename T>
	const T& operator[](size_t _index) const
	{
		const size_t indexInSparse = m_SparseList.at(_index);
		return m_Dense.at(indexInSparse);
	}

private:
	static constexpr  uint32_t NULL_INDEX = -1;

	
	/**
	 * Size of one item store
	 */
	size_t m_Density;
	
	std::vector<uint8_t> m_Dense;

	std::vector<EntityId> m_SparseList;

	SparsetCreateFunc m_CreateFunc;

	SparsetDeleteFunc m_DeleteFunc;
};




END_PCCORE
