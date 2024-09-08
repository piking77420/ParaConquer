#pragma once

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
        PC_CORE_API std::vector<uint8_t>* GetComponentData(uint32_t _componentKey);

        PC_CORE_API const std::vector<uint8_t>* GetComponentData(uint32_t _componentKey) const;
    
        PC_CORE_API uint8_t* GetComponent(EntityId _entityID, uint32_t _componentKey);

        PC_CORE_API const uint8_t* GetComponent(EntityId _entityID, uint32_t _componentKey) const;

        PC_CORE_API uint8_t* CreateComponent(EntityId _entityID, uint32_t _componentKey);

        PC_CORE_API void DeleteComponent(EntityId _entityID, uint32_t _componentKey);
        
        PC_CORE_API EntityId CreateEntity();

        PC_CORE_API void DestroyEntity(EntityId entityId);

        PC_CORE_API bool IsEntityIdValid(EntityId entityId) const;

        PC_CORE_API bool IsEntityHasComponent(EntityId entityId, uint32_t _componentKey) const;

        PC_CORE_API EntityRegister();

        PC_CORE_API ~EntityRegister() = default;

    private:
        
        
        std::array<EntityId, MAX_ENTITIES> m_Entities;

        std::vector<SparsetKey> sparseSets;

        SparseSet* GetSparsetFromKey(uint32_t _key);

        const SparseSet* GetSparsetFromKey(uint32_t _key) const;
    };


END_PCCORE
