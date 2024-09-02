#pragma once

#include <array>
#include <string>
#include <vector>

#include "core_header.hpp"
#include "ecs_header.h"
#include "sparse_sets.hpp"

BEGIN_PCCORE

    class EntityRegister
    {
    public:
        uint8_t* GetComponentData(uint32_t _componentKey);

        uint8_t* GetComponent(EntityId _entityID, uint32_t _componentKey);

        const uint8_t* GetComponent(EntityId _entityID, uint32_t _componentKey) const;

        uint8_t* CreateComponent(EntityId _entityID, uint32_t _componentKey);

        void DeleteComponent(EntityId _entityID, uint32_t _componentKey);
        
        EntityId CreateEntity();

        void DestroyEntity(EntityId entityId);

        EntityRegister();

        ~EntityRegister() = default;

    private:
        struct SparsetKey
        {
            uint32_t key;
            SparseSet sparse;
        };
        std::array<EntityId, MAX_ENTITIES> m_Entities;

        std::vector<SparsetKey> sparseSets;
    };

END_PCCORE
