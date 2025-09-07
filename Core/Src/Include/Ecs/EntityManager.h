#pragma once

#include <Array>
#include <String>
#include <Vector>
#include <Queue>

#include "Component.h"
#include "EcsSystem.h"
#include "CoreHeader.hpp"
#include "EcsHeader.h"
#include "EcsSystem.h"


BEGIN_PCCORE
    class EntityManager
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(EntityManager)

        PC_CORE_API EntityManager() = default;

        PC_CORE_API ~EntityManager() = default;

        PC_CORE_API EntityId CreateEntity();

        PC_CORE_API EntityId CreateEntity(const std::string& _name);

        PC_CORE_API std::string_view GetEntityName(EntityId) const;
    
        PC_CORE_API void RemoveEntity(EntityId entityId);

        PC_CORE_API void SetSignature(EntityId entityId, const Signature& signature);

        PC_CORE_API Signature* GetSignature(EntityId entity);

        PC_CORE_API const Signature* GetSignature(EntityId entity) const;

        PC_CORE_API PC_FORCE_INLINE bool IsValid(EntityId _id) const
        {
            return m_EntityEnableFlags.test(_id);
        }
    private:
    uint32_t m_LivingEntityCount = 0;

    SpareSet<Signature> m_EntitesSignature;

    // this queu is a queu of available entity id
    std::queue<EntityId> m_AvailableEntitiesId;

    uint32_t m_EntityIdCounter = 0;

    SpareSet<std::array<char, 255>> m_EntityNameAlloc;
    
    std::bitset<MAX_ENTITIES> m_EntityEnableFlags;
    
    REFLECT(EntityManager)
    REFLECT_MEMBER(EntityManager, m_LivingEntityCount);
    REFLECT_MEMBER(EntityManager, m_EntitesSignature);
    REFLECT_MEMBER(EntityManager, m_AvailableEntitiesId);
    REFLECT_MEMBER(EntityManager, m_EntityIdCounter);
    REFLECT_MEMBER(EntityManager, m_EntityNameAlloc);
    REFLECT_MEMBER(EntityManager, m_EntityEnableFlags);


    };


END_PCCORE
