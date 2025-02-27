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

BEGIN_PCCORE

    class EntityManager
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(EntityManager)

        PC_CORE_API EntityManager();

        PC_CORE_API ~EntityManager();

        PC_CORE_API EntityId CreateEntity();

        PC_CORE_API void RemoveEntity(EntityId entityId);

        PC_CORE_API void SetSignature(EntityId entityId, const Signature& signature);

        PC_CORE_API Signature& GetSignature(EntityId entity);

        PC_CORE_API const Signature& GetSignature(EntityId entity) const;
    private:

        uint32_t m_LivingEntityCount = 0;

        std::vector<Signature> m_EntitesSignature;
        
        std::vector<EntityId> m_Entities;

        std::queue<EntityId> m_AvailableEntitiesId;
       
    };



END_PCCORE
