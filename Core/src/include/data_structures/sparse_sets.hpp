#pragma once
#include <vector>

#include "core_header.hpp"

BEGIN_PCCORE

#define PAGE_SIZE 10
#define NULL_INDEX std::numeric_limits<std::size_t>::max()

using SparsetCreateFunc = void (*)(void*);
using SparsetDeleteFunc = void (*)(void*);

class SparseSet
{
public:
	DEFAULT_COPY_MOVE_OPERATIONS(SparseSet)

	PC_CORE_API std::vector<uint8_t>* GetData();

	PC_CORE_API const std::vector<uint8_t>* GetData() const;
	
	PC_CORE_API inline size_t GetSize() const
	{
		return m_Dense.size() / m_Density;
	}

	PC_CORE_API inline  size_t GetDensity() const
	{
		return m_Density;
	}

	PC_CORE_API uint8_t* Alloc(size_t _index);

	PC_CORE_API void Free(size_t _index);

	PC_CORE_API const uint8_t* At(size_t _index) const;

	PC_CORE_API uint8_t* At(size_t _index);

	PC_CORE_API bool Empty() const;

	PC_CORE_API bool Contains(size_t _index) const;

	PC_CORE_API SparseSet() = default;

	PC_CORE_API ~SparseSet() = default;
	
	PC_CORE_API SparseSet(size_t _densitySize);

	PC_CORE_API void Clear();
	
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
	
	PC_FORCE_INLINE uint8_t& operator[](size_t _index)
	{
		const size_t pageIndex = _index / PAGE_SIZE;
		const size_t offset = _index % PAGE_SIZE;
		
		return m_Dense.at(m_SparseList[pageIndex][offset]);
	}
	PC_FORCE_INLINE const uint8_t& operator[](size_t _index) const
	{
		const size_t pageIndex = _index / PAGE_SIZE;
		const size_t offset = _index % PAGE_SIZE;
		
		return m_Dense.at(m_SparseList[pageIndex][offset]);
	}

private:
	/**
	 * Size of one item store
	 */
	size_t m_Density;
	
	std::vector<uint8_t> m_Dense;

	std::vector<std::vector<size_t>> m_SparseList;
};




END_PCCORE
