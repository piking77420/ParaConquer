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

EntityId Scene::CreateEntity(const std::string& name)
{
    const Entity entity =
        { 
        .name = name,
        .ecsId = m_EntityRegister.CreateEntity()
        };
    m_Entities.push_back(entity);

    return m_Entities.at(m_Entities.size() - 1).ecsId;
}

const Entity* Scene::GetEntity(EntityId _id) const
{
    if (!m_EntityRegister.IsEntityIdValid(_id))
        return nullptr;

    return &*std::ranges::find_if(m_Entities, [_id](const Entity& e){return e.ecsId == _id;});
}

Entity* Scene::GetEntity(EntityId _id)
{
    if (!m_EntityRegister.IsEntityIdValid(_id))
        return nullptr;

    return &*std::ranges::find_if(m_Entities, [_id](const Entity& e){return e.ecsId == _id;});
}

void* Scene::Get(EntityId _entityId, uint32_t _componentKey)
{
    return reinterpret_cast<void*>(m_EntityRegister.GetComponent(_entityId, _componentKey));
}

void Scene::RemoveComponent(EntityId _entityId, uint32_t _componentKey)
{
    m_EntityRegister.DeleteComponent(_entityId, _componentKey);
}

bool Scene::HasComponent(EntityId _entityId, uint32_t _componentKey) const
{
    return m_EntityRegister.IsEntityHasComponent(_entityId, _componentKey);
}

void PC_CORE::Scene::AddComponent(EntityId _entityId, uint32_t _componentKey)
{
    m_EntityRegister.CreateComponent(_entityId, _componentKey);
}

