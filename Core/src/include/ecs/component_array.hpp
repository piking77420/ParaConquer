#pragma once

#include "core_header.hpp"
#include "ecs_header.h"

using Constructor = void(*)(void*);
using Destructor = void(*)(void*);

BEGIN_PCCORE

struct ComponentArrayCreateInfo
{
	uint32_t componentSize;
	Constructor constructor;
	Destructor destructor;
};

class ComponentArray
{
public:
	PC_CORE_API ComponentArray();

	PC_CORE_API ComponentArray(const ComponentArrayCreateInfo& componentArrayCreateInfo);

	PC_CORE_API void AddEntityData(EntityId entityId);

	PC_CORE_API uint8_t& GetData(EntityId entityId);

	PC_CORE_API const uint8_t& GetData(EntityId entityId) const ;

	PC_CORE_API void RemoveEntityData(EntityId entityId);

	PC_CORE_API bool HasComponent(EntityId entityId) const;

private:
	PC_FORCE_INLINE void PushData(EntityId entityId);

	PC_FORCE_INLINE void RemoveData(EntityId entityId);

	ComponentArrayCreateInfo m_ComponentArrayInfo;	

	std::vector<uint8_t> m_ComponentData;

	std::unordered_map<EntityId, size_t> m_EntityToIndex;

	std::unordered_map<EntityId, size_t> m_IndexToEntity;

	size_t m_Volume;
};

END_PCCORE