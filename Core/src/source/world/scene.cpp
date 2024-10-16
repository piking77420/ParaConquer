﻿#include "world\scene.hpp"

#include <cassert>

#include "log.hpp"
#include "world/component.hpp"
#include "world/component_register.h"

using namespace PC_CORE;

Entity Scene::CreateEntity()
{
    for (uint32_t i = 0; i < m_entities.size(); ++i)
    {
        if (!m_entities[i].isEnable)
        {
            m_entities[i].isEnable = true;
            m_entities[i].name = "Entity " + std::to_string(i);
            
            return i;
        }
    }

    PC_LOGERROR("There is no available entity index")
    return NULL_ENTITY;
}

bool Scene::RemoveEntity(Entity _entity)
{
    if (!m_entities.at(_entity).isEnable || _entity >= MAX_ENTITIES)
    {
        PC_LOGERROR("Entity is not valid")
        return false;
    }

    for (uint32_t i = 0; i < ComponentRegister::GetNbrOfComponentType(); i++)
    {
        RemoveComponentInternal(i, _entity);
    }

    return true;
}

void Scene::GetComponentDataRaw(uint32_t _componentiD, const std::vector<uint8_t>** _data) const
{
    assert(_componentiD >= 0);
    *_data = &m_ComponentData.at(_componentiD);
}

void Scene::GetComponentDataRaw(uint32_t _componentiD, std::vector<uint8_t>** _data)
{
    assert(_componentiD >= 0);
    *_data = &m_ComponentData.at(_componentiD);
}

void Scene::Update()
{
    for (System* sys: systems)
    {
        sys->Update(this);
    }
}

void Scene::Begin()
{
    for (System* sys: systems)
    {
        sys->Begin(this);
    }
}

Scene::EntityInternal* Scene::GetEntityInternal(Entity _entity)
{
    if (!m_entities.at(_entity).isEnable)
        return nullptr;

    return &m_entities.at(_entity);
}

const Scene::EntityInternal* Scene::GetEntityInternal(Entity _entity) const
{
    if (!m_entities.at(_entity).isEnable)
        return nullptr;

    return &m_entities.at(_entity);
}

Scene::Scene()
{
    // init Component Data
    m_ComponentData.resize(ComponentRegister::GetNbrOfComponentType());
    const uint32_t NbrOfComponentType = ComponentRegister::GetNbrOfComponentType();

    for (size_t i = 0; i < NbrOfComponentType; i++)
    {
        const size_t sizeOfComponent = ComponentRegister::GetSize(static_cast<uint32_t>(i));
        m_ComponentData[i].resize(sizeOfComponent * MAX_ENTITIES);

        for (size_t k = 0; k < m_ComponentData[i].size(); k += sizeOfComponent)
        {
            Component* componentPtr = reinterpret_cast<Component*>(&m_ComponentData[i][k]);
            componentPtr->entityID = NULL_ENTITY;
        }
    }

    // init entities
    for (EntityInternal& entity : m_entities)
    {
        entity.componentIdIndexInDataArray.resize(NbrOfComponentType);
        for (size_t i = 0; i < NbrOfComponentType; i++)
        {
            entity.componentIdIndexInDataArray[i] = NULL_COMPONENT;
        }
    }
}


Scene::~Scene()
{
    for (size_t ent = 0; ent < m_entities.size(); ent++)
    {
        if (!m_entities[ent].isEnable)
            continue;
            
        for (size_t comp = 0; comp < m_entities[ent].componentIdIndexInDataArray.size(); comp++)
        {
            RemoveComponentInternal(static_cast<uint32_t>(comp), static_cast<uint32_t>(ent));
        }
    }
}

Component* Scene::AddComponentInternal(uint32_t _componentId, Entity _entity)
{
    if (!m_entities.at(_entity).isEnable)
    {
        //PC_LOGERROR("This entity is not enable")
        return nullptr;
    }

    if (HadComponent(_entity, _componentId, nullptr))
    {
        //PC_LOGERROR("This entity has alrealy this component")
        return nullptr;
    }

    std::vector<uint8_t>* componentArray = &(m_ComponentData[_componentId]);
    const size_t sizeComponent = ComponentRegister::GetSize(_componentId);

    for (size_t i = 0; i < componentArray->size(); i += sizeComponent)
    {
        Component* component = reinterpret_cast<Component*>(&((*componentArray)[i]));
        // To be component valid to add the component entity id need to be Null
        if (component->entityID == NULL_ENTITY)
        {
            component->entityID = _entity;
            // add  the compoentn index to the entity
            m_entities.at(_entity).componentIdIndexInDataArray[_componentId] = static_cast<uint32_t>(i);
            const CreateFunc createFunc = ComponentRegister::GetCreateFunc(_componentId);
            const size_t index = m_entities.at(_entity).componentIdIndexInDataArray[_componentId];

            createFunc(componentArray,index, _entity);
            return reinterpret_cast<Component*>(&componentArray->at(index));
        }
    }
    PC_LOGERROR("Souldn't be here at Add component")
    return nullptr;
}

Component* Scene::GetComponentInternal(uint32_t _componentId, Entity _entity)
{
    if (!m_entities.at(_entity).isEnable)
    {
        PC_LOGERROR("This entity is not enable")
        return nullptr;
    }
    uint32_t indexInData;
    if (!HadComponent(_entity, _componentId, &indexInData))
    {
        PC_LOGERROR("This entity doen't have this component")
        return nullptr;
    }
    return reinterpret_cast<Component*>(&m_ComponentData[_componentId].at(indexInData));
}

const Component* Scene::GetComponentInternal(uint32_t _componentId, Entity _entity) const
{
    if (!m_entities.at(_entity).isEnable)
    {
        PC_LOGERROR("This entity is not enable")
        return nullptr;
    }

    uint32_t indexInData;
    if (!HadComponent(_entity, _componentId, &indexInData))
    {
        PC_LOGERROR("This entity doen't have this component")
        return nullptr;
    }

    return reinterpret_cast<const Component*>(&m_ComponentData.at(_componentId).at(indexInData));
}

bool Scene::RemoveComponentInternal(uint32_t _componentId, Entity _entity)
{
    if (!m_entities.at(_entity).isEnable)
    {
        return false;
    }

    uint32_t index;
    if (!HadComponent(_entity, _componentId, &index))
    {
        return false;
    }
    std::vector<uint8_t>* componentArray = &m_ComponentData[_componentId];
    const DeleteFunc deleteFunc = ComponentRegister::GetDeleteFunc(_componentId);
    
    deleteFunc(&componentArray->at(index));
    m_entities.at(_entity).componentIdIndexInDataArray.at(_componentId) = NULL_COMPONENT;
    
    Component* c = reinterpret_cast<Component*>(&componentArray->at(index));
    c->entityID = NULL_ENTITY;
    
    return true;
}



bool Scene::HadComponent(Entity _entity, uint32_t _componentId, uint32_t* _outIndexinDataArray) const
{
    if (m_entities.at(_entity).componentIdIndexInDataArray[_componentId] == NULL_COMPONENT)
    {
        if (_outIndexinDataArray)
        {
            *_outIndexinDataArray = NULL_COMPONENT;
        }

        return false;
    }

    if (_outIndexinDataArray != nullptr)
        *_outIndexinDataArray = m_entities.at(_entity).componentIdIndexInDataArray.at(_componentId);

    return true;
}
