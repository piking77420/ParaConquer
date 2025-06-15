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

	

private:
	PC_FORCE_INLINE void PushData(EntityId entityId);

	PC_FORCE_INLINE void RemoveData(EntityId entityId);

	PC_CORE_API void GetComponentInfo();
	
	std::vector<uint8_t> m_ComponentData;

	TypeId componentType;

	uint32_t componentSize;

	size_t m_Volume = 0;

	// TO DO CHANGE FOR VECTOR
	std::unordered_map<EntityId, size_t> m_EntityToIndex;

	std::unordered_map<EntityId, size_t> m_IndexToEntity;


	Constructor constructor;

	Destructor destructor;

	REFLECT(ComponentArray)
	REFLECT_MEMBER(ComponentArray, m_ComponentData)
	REFLECT_MEMBER(ComponentArray, componentType)
	REFLECT_MEMBER(ComponentArray, m_Volume)
	REFLECT_MEMBER(ComponentArray, m_IndexToEntity)
	REFLECT_MEMBER(ComponentArray, m_EntityToIndex)

};





END_PCCORE