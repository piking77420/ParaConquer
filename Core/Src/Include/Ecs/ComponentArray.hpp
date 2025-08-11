#pragma once

#include "CoreHeader.hpp"
#include "EcsHeader.h"

using Constructor = void(*)(void*);
using Destructor = void(*)(void*);

BEGIN_PCCORE


class ComponentArray
{
public:
	PC_CORE_API ComponentArray();

	PC_CORE_API ComponentArray(TypeId componentType);

	PC_CORE_API void Add(EntityId entityId);

	PC_CORE_API uint8_t& Get(EntityId entityId);

	PC_CORE_API const uint8_t& Get(EntityId entityId) const ;

	PC_CORE_API void Remove(EntityId entityId);

private:
	std::vector<uint8_t> m_ComponentData;

	std::vector<size_t> m_EntityIndexData;

	TypeId m_ComponentType;

	uint32_t m_ComponentSize;

	size_t m_Volume = 0;

	Constructor constructor = nullptr;

	Destructor destructor = nullptr;

	PC_CORE_API bool HasComponent(EntityId entityId) const;

	REFLECT(ComponentArray)
	REFLECT_MEMBER(ComponentArray, m_ComponentData)
	REFLECT_MEMBER(ComponentArray, m_EntityIndexData)
	REFLECT_MEMBER(ComponentArray, m_ComponentSize)

	REFLECT_MEMBER(ComponentArray, m_ComponentType)
	REFLECT_MEMBER(ComponentArray, m_Volume)
};





END_PCCORE