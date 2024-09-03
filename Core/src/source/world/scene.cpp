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
    if (!m_EntityRegister.IsValid(_id))
        return nullptr;

    return &*std::ranges::find_if(m_Entities, [_id](const Entity& e){return e.ecsId == _id;});
}

Entity* Scene::GetEntity(EntityId _id)
{
    if (!m_EntityRegister.IsValid(_id))
        return nullptr;

    return &*std::ranges::find_if(m_Entities, [_id](const Entity& e){return e.ecsId == _id;});
}

uint8_t* Scene::GetData(uint32_t _componentKey, size_t _componentSize, size_t* _bufferSizeT)
{
    size_t dataSize = 0; 
    uint8_t* data = m_EntityRegister.GetComponentData(_componentKey, &dataSize);
    *_bufferSizeT = dataSize / _componentSize;
    return data;
}

