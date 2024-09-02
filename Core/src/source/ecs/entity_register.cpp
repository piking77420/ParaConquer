#include "ecs/entity_register.h"

#include "log.hpp"
#include "ecs/ecs_context.h"

uint8_t* PC_CORE::EntityRegister::GetComponentData(uint32_t _componentKey)
{
    return sparseSets.at(_componentKey).sparse.GetData();
}

uint8_t* PC_CORE::EntityRegister::GetComponent(EntityId _entityID, uint32_t _componentKey)
{
    return sparseSets.at(_componentKey).sparse.GetEntityData(_entityID);   
}

const uint8_t* PC_CORE::EntityRegister::GetComponent(EntityId _entityID, uint32_t _componentKey) const
{
    return sparseSets.at(_componentKey).sparse.GetEntityData(_entityID);   
}

uint8_t* PC_CORE::EntityRegister::CreateComponent(EntityId _entityID, uint32_t _componentKey)
{
    return sparseSets.at(_componentKey).sparse.Alloc(_entityID);
}

void PC_CORE::EntityRegister::DeleteComponent(EntityId _entityID, uint32_t _componentKey)
{
    return sparseSets.at(_componentKey).sparse.Free(_entityID);
}

EntityId PC_CORE::EntityRegister::CreateEntity()
{
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        if (i != INVALID_ENTITY_ID)
        {
            m_Entities.at(i) = static_cast<uint32_t>(i);
            return static_cast<uint32_t>(i);
        }
    }

    PC_LOGERROR("Failed to create entity , Max has been reach");
    return INVALID_ENTITY_ID;
}

void PC_CORE::EntityRegister::DestroyEntity(EntityId entityId)
{
    for (size_t i = 0; i < sparseSets.size(); ++i)
    {
        sparseSets.at(i).sparse.Free(entityId);
    }

    m_Entities.at(entityId) = INVALID_ENTITY_ID;
}

PC_CORE::EntityRegister::EntityRegister() 
{
    const std::vector<EcsComponent>& components = EcsContext::GetComponentsDataInfo();
    for (const EcsComponent& ecsComponent : components)
    {
        sparseSets.push_back({ecsComponent.key, SparseSet(ecsComponent.size)});
    }
    
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        m_Entities.at(i) = INVALID_ENTITY_ID;
    }
}
