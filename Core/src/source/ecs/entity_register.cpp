#include "ecs/entity_register.h"

#include "log.hpp"
#include "ecs/component.h"
#include "ecs/ecs_context.h"

uint8_t* PC_CORE::EntityRegister::GetComponentData(uint32_t _componentKey, size_t* _size)
{
    SparseSet* sparseSet = GetSparsetFromKey(_componentKey);
    *_size = sparseSet->GetSize();
    return sparseSet->GetData();
}

const uint8_t* PC_CORE::EntityRegister::GetComponentData(uint32_t _componentKey, size_t* _size) const
{
    const SparseSet* sparseSet = GetSparsetFromKey(_componentKey);
    *_size = sparseSet->GetSize();
    return sparseSet->GetData();
}

uint8_t* PC_CORE::EntityRegister::GetComponent(EntityId _entityID, uint32_t _componentKey)
{
    return GetSparsetFromKey(_componentKey)->GetEntityData(_entityID);   
}

const uint8_t* PC_CORE::EntityRegister::GetComponent(EntityId _entityID, uint32_t _componentKey) const
{
    return GetSparsetFromKey(_componentKey)->GetEntityData(_entityID);   
}

uint8_t* PC_CORE::EntityRegister::CreateComponent(EntityId _entityID, uint32_t _componentKey)
{
    Component* newComponent = reinterpret_cast<Component*>(GetSparsetFromKey(_componentKey)->Alloc(_entityID));
    newComponent->entityId = _entityID;
    return reinterpret_cast<uint8_t*>(newComponent);
}

void PC_CORE::EntityRegister::DeleteComponent(EntityId _entityID, uint32_t _componentKey)
{
    return GetSparsetFromKey(_componentKey)->Free(_entityID);
}

EntityId PC_CORE::EntityRegister::CreateEntity()
{
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        if (m_Entities.at(i) == INVALID_ENTITY_ID)
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
    for (auto& sparseSet : sparseSets)
    {
        sparseSet.sparse.Free(entityId);
    }

    m_Entities.at(entityId) = INVALID_ENTITY_ID;
}

bool PC_CORE::EntityRegister::IsEntityIdValid(EntityId entityId) const
{
    return m_Entities.at(entityId) != INVALID_ENTITY_ID;
}

bool PC_CORE::EntityRegister::IsEntityHasComponent(EntityId entityId, uint32_t _componentKey) const
{
    return GetSparsetFromKey(_componentKey)->GetEntityData(entityId) != nullptr;
}

PC_CORE::EntityRegister::EntityRegister() 
{
    const std::vector<EcsComponent>& components = EcsContext::GetComponentsDataInfo();
    for (const EcsComponent& ecsComponent : components)
    {
        sparseSets.push_back({ecsComponent.key, SparseSet(ecsComponent.size, MAX_ENTITIES, ecsComponent.Createfunc, ecsComponent.Deletefunc )});
    }
    
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        m_Entities.at(i) = INVALID_ENTITY_ID;
    }
}

PC_CORE::SparseSet* PC_CORE::EntityRegister::GetSparsetFromKey(uint32_t _key)
{
    auto it = std::ranges::find_if(sparseSets, [_key](const SparsetKey& sparsetKey)
   {
       return sparsetKey.key == _key;
   });

    if (it != sparseSets.end()) // Check if the iterator is valid
    {
        return &it->sparse;
    }

    return nullptr; 
}

const PC_CORE::SparseSet* PC_CORE::EntityRegister::GetSparsetFromKey(uint32_t _key) const
{
    auto it = std::ranges::find_if(sparseSets, [_key](const SparsetKey& sparsetKey)
   {
       return sparsetKey.key == _key;
   });

    if (it != sparseSets.end()) // Check if the iterator is valid
    {
        return &it->sparse;
    }

    return nullptr; 
}
