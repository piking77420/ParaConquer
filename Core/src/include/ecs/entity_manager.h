#pragma once

#include <array>
#include <string>
#include <vector>
#include <queue>

#include "component.h"
#include "ecs_system.h"
#include "core_header.hpp"
#include "ecs_header.h"
#include "ecs_system.h"
#include "serialize/iseriazable.h"


BEGIN_PCCORE
    class EntityManager : public ISeriazable
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(EntityManager)

        IMP_DYNAMIC_REFLECT();

        PC_CORE_API EntityManager();

        PC_CORE_API ~EntityManager() override = default;

        PC_CORE_API EntityId CreateEntity();

        PC_CORE_API EntityId CreateEntity(const std::string& _name);

        PC_CORE_API std::string_view GetEntityName(EntityId) const;
    
        PC_CORE_API void RemoveEntity(EntityId entityId);

        PC_CORE_API void SetSignature(EntityId entityId, const Signature& signature);

        PC_CORE_API Signature* GetSignature(EntityId entity);

        PC_CORE_API const Signature* GetSignature(EntityId entity) const;

       
    private:
    uint32_t m_LivingEntityCount = 0;

    SpareSet<Signature> m_EntitesSignature;

    // to do reflect and serializable queu and stack
    // replace with stack
    std::queue<EntityId> m_AvailableEntitiesId;

    SpareSet<std::array<char, 255>> m_EntityNameAlloc;
    
    std::bitset<MAX_ENTITIES> m_EntityEnableFlags;
    
    REFLECT(EntityManager)
    REFLECT_MEMBER(EntityManager, m_LivingEntityCount);
    REFLECT_MEMBER(EntityManager, m_EntitesSignature);
    REFLECT_MEMBER(EntityManager, m_AvailableEntitiesId);
    REFLECT_MEMBER(EntityManager, m_EntityNameAlloc);
    REFLECT_MEMBER(EntityManager, m_EntityEnableFlags);


    };


END_PCCORE
