#include "ecs/entity_register.h"

#include "log.hpp"
#include "ecs/component.h"


const PC_CORE::Entity& PC_CORE::EntityRegister::CreateEntity() const
{
    const Entity* entity = nullptr;
    
    for (size_t i = 0; i < entities.size(); ++i)
    {
        if (entities.at(i).id == INVALID_ENTITY_ID)
        {
            entities.at(i).id = static_cast<uint32_t>(i);
            entity = &entities.at(i);
        }
    }

    if (entity == nullptr)
    {
        PC_LOGERROR("Failed to create entity , Max has been reach");
    }
    return *entity;
}

PC_CORE::Entity& PC_CORE::EntityRegister::CreateEntity()
{
    Entity* entity = nullptr;
    
    for (size_t i = 0; i < entities.size(); ++i)
    {
        if (entities.at(i).id == INVALID_ENTITY_ID)
        {
            entities.at(i).id = static_cast<uint32_t>(i);
            entity = &entities.at(i);
            return *entity;
        }
    }

    PC_LOGERROR("Failed to create entity , Max has been reach");
    
    return *entity;
}

void PC_CORE::EntityRegister::DestroyEntity(const Entity& _entity)
{
    
    for (auto& set : m_SparseSetsMap)
    {
        if (set.second.IsValid(_entity.id))
            set.second.Free(_entity.id);
    }
}

PC_CORE::EntityRegister::EntityRegister() 
{
    std::vector<const ReflectedType*> m_ComponentsType = Reflector::GetAllTypesFrom<Component>();

    m_SparseSetsMap.reserve(m_ComponentsType.size());
    
    for (const ReflectedType* ecsComponent : m_ComponentsType)
    {
        m_SparseSetsMap.emplace(ecsComponent->HashKey, *ecsComponent);
    }
}


