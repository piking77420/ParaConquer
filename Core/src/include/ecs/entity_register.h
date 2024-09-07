﻿#pragma once

#include <array>
#include <string>
#include <vector>

#include "core_header.hpp"
#include "ecs_header.h"
#include "sparse_sets.hpp"

BEGIN_PCCORE

    struct SparsetKey
    {
        uint32_t key;
        SparseSet sparse;
    };
    REFLECT(SparsetKey)
    REFLECT_MEMBER(SparsetKey, key)
           

    class EntityRegister
    {
    public:
        std::vector<uint8_t>* GetComponentData(uint32_t _componentKey);

        const std::vector<uint8_t>* GetComponentData(uint32_t _componentKey) const;
    
        uint8_t* GetComponent(EntityId _entityID, uint32_t _componentKey);

        const uint8_t* GetComponent(EntityId _entityID, uint32_t _componentKey) const;

        uint8_t* CreateComponent(EntityId _entityID, uint32_t _componentKey);

        void DeleteComponent(EntityId _entityID, uint32_t _componentKey);
        
        EntityId CreateEntity();

        void DestroyEntity(EntityId entityId);

        bool IsEntityIdValid(EntityId entityId) const;

        bool IsEntityHasComponent(EntityId entityId, uint32_t _componentKey) const;

        EntityRegister();

        ~EntityRegister() = default;

    private:
        
        
        std::array<EntityId, MAX_ENTITIES> m_Entities;

        std::vector<SparsetKey> sparseSets;

        SparseSet* GetSparsetFromKey(uint32_t _key);

        const SparseSet* GetSparsetFromKey(uint32_t _key) const;
    };


END_PCCORE
