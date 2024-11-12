#include "world\scene.hpp"

#include <cassert>

#include "log.hpp"
//#include "world/component.hpp"
//#include "world/component_register.h"

using namespace PC_CORE;

void Scene::Begin()
{
}

void Scene::Update()
{
}

Entity* Scene::CreateEntity(const std::string& name)
{
    Entity entity;
    entity.name = name,
    entity.ecsId = m_EntityRegister.CreateEntity();

    m_Entities.push_back(entity);

    return &m_Entities.at(m_Entities.size() - 1);
}

const Entity* Scene::GetEntityFromId(EntityId _entityId) const
{
    if (!m_EntityRegister.IsEntityIdValid(_entityId))
        return nullptr;

    return &*std::ranges::find_if(m_Entities, [&](const Entity& e) {return e.ecsId == _entityId; });
}

Entity* Scene::GetEntityFromId(EntityId _entityId)
{
    if (!m_EntityRegister.IsEntityIdValid(_entityId))
        return nullptr;

    return &*std::ranges::find_if(m_Entities, [&](const Entity& e){return e.ecsId == _entityId;});
}

void* Scene::GetComponent(Entity* _entity, uint32_t _componentKey)
{
    return reinterpret_cast<void*>(m_EntityRegister.GetComponent(_entity->ecsId, _componentKey));
}

void Scene::RemoveComponent(Entity* _entity, uint32_t _componentKey)
{
    m_EntityRegister.DeleteComponent(_entity->ecsId, _componentKey);
}

bool Scene::HasComponent(Entity* _entity, uint32_t _componentKey) const
{
    return m_EntityRegister.IsEntityHasComponent(_entity->ecsId, _componentKey);
}

void PC_CORE::Scene::AddComponent(Entity* _entity, uint32_t _componentKey)
{
    m_EntityRegister.CreateComponent(_entity->ecsId, _componentKey);
}

