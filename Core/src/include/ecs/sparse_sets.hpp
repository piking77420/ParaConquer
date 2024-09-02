#pragma once
#include <vector>

#include "core_header.hpp"
#include "entity_register.h"
#include "ecs/ecs_header.h"

BEGIN_PCCORE
	class SparseSet
{
public:
	uint8_t* GetData();

	const uint8_t* GetData() const;

	size_t GetSize() const;

	size_t GetDensity() const;

	void Alloc(EntityId entity_id);
	
	SparseSet(size_t _densitySize);

	SparseSet(size_t _densitySize, size_t _size);

	SparseSet(size_t _densitySize, size_t _size, CreateFunc _createFunc, DeleteFunc _deleteFunc);

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
	size_t m_Density;
	
	std::vector<uint8_t> m_Dense;

	std::vector<EntityId> m_SparseList;

	CreateFunc m_CreateFunc;

	DeleteFunc m_DeleteFunc;
};




END_PCCORE
