#pragma once

#include "core_header.hpp"
#include "ecs_header.h"

using Constructor = void(*)(void*);
using Destructor = void(*)(void*);

BEGIN_PCCORE

class ComponentArray
{
public:
	PC_CORE_API ComponentArray();

	PC_CORE_API ComponentArray(TypeId componentType);

	PC_CORE_API void AddEntityData(EntityId entityId);

	PC_CORE_API uint8_t& GetData(EntityId entityId);

	PC_CORE_API const uint8_t& GetData(EntityId entityId) const ;

	PC_CORE_API void RemoveEntityData(EntityId entityId);

	PC_CORE_API bool HasComponent(EntityId entityId) const;

	uint8_t watcherValue = std::numeric_limits<uint8_t>::max();

	std::vector<uint8_t> m_ComponentData;

	TypeId componentType;

	size_t m_Volume = 0;

	std::unordered_map<EntityId, size_t> m_EntityToIndex;

	std::unordered_map<EntityId, size_t> m_IndexToEntity;

	uint32_t componentSize;

	Constructor constructor;

	Destructor destructor;

private:
	PC_FORCE_INLINE void PushData(EntityId entityId);

	PC_FORCE_INLINE void RemoveData(EntityId entityId);

	PC_CORE_API void GetComponentInfo();

};


constexpr size_t off = offsetof(ComponentArray, m_ComponentData);


REFLECT(ComponentArray)
REFLECT_MEMBER(ComponentArray, m_ComponentData)
//REFLECT_MEMBER(ComponentArray, componentType)
//REFLECT_MEMBER(ComponentArray, m_Volume)
//REFLECT_MEMBER(ComponentArray, m_IndexToEntity)
//REFLECT_MEMBER(ComponentArray, m_EntityToIndex)


END_PCCORE