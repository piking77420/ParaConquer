#include "ecs/entity_manager.h"

#include <numbers>

#include "log.hpp"
#include "ecs/component.h"

using namespace PC_CORE;

EntityId PC_CORE::EntityManager::CreateEntity()
{
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        if (m_Entities.at(i).id == INVALID_ENTITY_ID)
        {
            m_Entities.at(i).id = static_cast<uint32_t>(i);
            return static_cast<EntityId>(i);
        }
    }

    if (m_Entities.size() + 1 >= MAX_ENTITY)
    {
        PC_LOGERROR("Failed to create entity , Max has been reach");
        throw std::runtime_error("Failed to create entity , Max has been reach");
    }

    const EntityId id = static_cast<EntityId>(m_Entities.size());
    m_Entities.emplace_back(id, "Entity " + std::to_string(id));
    m_EntityCount++;
    return id;
}

EntityId PC_CORE::EntityManager::CreateEntity(std::string&& name)
{
    for (size_t i = 0; i < m_Entities.size(); ++i)
    {
        if (m_Entities.at(i).id == INVALID_ENTITY_ID)
        {
            m_Entities.at(i).id = static_cast<uint32_t>(i);
            return static_cast<EntityId>(i);
        }
    }

    if (m_Entities.size() + 1 >= MAX_ENTITY)
    {
        PC_LOGERROR("Failed to create entity , Max has been reach");
        throw std::runtime_error("Failed to create entity , Max has been reach");
    }

    const EntityId id = static_cast<EntityId>(m_Entities.size());
    m_Entities.emplace_back(id, std::move(name));
    m_EntityCount++;
    return id;
}

bool PC_CORE::EntityManager::CreateEntity(EntityId _id, const std::string& name)
{
    if (m_Entities.size() < _id || m_Entities.size() > MAX_ENTITY || m_Entities.at(_id).id == INVALID_ENTITY_ID)
        return false;

    m_Entities.at(_id).id = static_cast<uint32_t>(_id);
    m_Entities.at(_id).name = name;
    m_EntityCount++;
    return true;
}

void PC_CORE::EntityManager::DestroyEntity(const Entity& _entity)
{
    m_EntityCount--;
    for (auto& set : m_SparseSetsMap)
    {
        if (set.second.IsValid(_entity.id))
            set.second.Free(_entity.id);
    }
}

PC_CORE::Entity& PC_CORE::EntityManager::GetEntity(EntityId _entityId)
{
    auto it = std::ranges::find_if(m_Entities, [_entityId](const Entity& _entity){return _entity.id == _entityId;});
    if (it == m_Entities.end())
    {
        PC_LOGERROR("Entity not found");
        throw std::runtime_error("Entity not found");
    }
    return *it;
}

const PC_CORE::Entity& PC_CORE::EntityManager::GetEntity(EntityId _entityId) const
{
    auto it = std::ranges::find_if(m_Entities, [_entityId](const Entity& _entity){return _entity.id == _entityId;});
    if (it == m_Entities.end())
    {
        PC_LOGERROR("Entity not found");
        throw std::runtime_error("Entity not found");
    }
    return *it;
}

Component* EntityManager::AddComponent(uint32_t typeKey, const EntityId& _entityId)
{
    TypeSparseSet& sparseSet = m_SparseSetsMap.at(typeKey);
    Component* ptr = reinterpret_cast<Component*>(sparseSet.Create(_entityId));

    if (ptr == nullptr)
        return ptr;

    const CreateFunc& createFunc = Reflector::GetType(typeKey).metaData.createFunc;

    ptr->entityId = _entityId;

    createFunc(ptr);
    return ptr;
    
}

void EntityManager::RemoveComponent(uint32_t typeKey, const EntityId& _entityId)
{
    TypeSparseSet& sparseSet = m_SparseSetsMap.at(typeKey);

    Component* component = GetComponent(typeKey, _entityId);
    DeleteFunc deleteFunc = Reflector::GetType(typeKey).metaData.deleteFunc;

    deleteFunc(component);
        
    sparseSet.Free(_entityId);
}

std::string PC_CORE::EntityManager::GetEntityName(EntityId _entityId) const
{
    const Entity& ent = GetEntity(_entityId);

    return ent.name;
}

std::string& EntityManager::GetEntityName(EntityId _entityId)
{
    return GetEntity(_entityId).name;
}

void PC_CORE::EntityManager::SetEntityName(EntityId _entityId, const std::string& _name)
{
    Entity& ent = GetEntity(_entityId);
    ent.name = ent.name;
}


PC_CORE::EntityManager::EntityManager() 
{
    m_Entities.reserve(RESERVE_ENTITIES);
    
    std::vector<const ReflectedType*> m_ComponentsType = Reflector::GetAllTypesFrom<Component>();

    m_SparseSetsMap.reserve(m_ComponentsType.size());
    
    for (const ReflectedType* ecsComponent : m_ComponentsType)
    {
        m_SparseSetsMap.emplace(ecsComponent->typeId, *ecsComponent);
    }
    
}


