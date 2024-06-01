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

Scene::Scene()
{
    // init Component Data
    m_ComponentData = new std::vector<uint8_t>[ComponentRegister::GetNbrOfComponentType()];
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
    for (EntityAndComponentIndex& entity : m_entities)
    {
        entity.componentIdIndexInDataArray = new uint32_t[NbrOfComponentType];
        for (size_t i = 0; i < NbrOfComponentType; i++)
        {
            entity.componentIdIndexInDataArray[i] = NULL_COMPONENT;
        }
    }
}

Scene::~Scene()
{
    for (EntityAndComponentIndex& entity : m_entities)
    {
        delete[] entity.componentIdIndexInDataArray;
        entity.componentIdIndexInDataArray = nullptr;
    }

    delete[] m_ComponentData;
}

Component* Scene::AddComponentInternal(uint32_t _componentId, Entity _entity)
{
    if (!m_entities.at(_entity).isEnable)
    {
        PC_LOGERROR("This entity is not enable")
        return nullptr;
    }

    if (HadComponent(_entity, _componentId, nullptr))
    {
        PC_LOGERROR("This entity has alrealy this component")
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
            return component;
        }
    }
    PC_LOGERROR("Souldn't be here at Add component")
    return nullptr;
}

bool Scene::RemoveComponentInternal(uint32_t _componentId, Entity _entity)
{
    if (!m_entities.at(_entity).isEnable)
    {
        PC_LOGERROR("This entity is not enable")
        return false;
    }

    if (!HadComponent(_entity, _componentId, nullptr))
    {
        PC_LOGERROR("This entity doesn't have this component")
        return false;
    }

    std::vector<uint8_t>* componentArray = &m_ComponentData[_componentId];
    const size_t sizeComponent = ComponentRegister::GetSize(_componentId);
    for (size_t i = 0; i < componentArray->size(); i += sizeComponent)
    {
        Component* component = reinterpret_cast<Component*>(&((*componentArray)[i]));
        // To be component valid to add the component entity id need to be Null
        if (component->entityID == _entity)
        {
            component->entityID = NULL_ENTITY;
            // add  the compoentn index to the entity
            m_entities.at(_entity).componentIdIndexInDataArray[_componentId] = NULL_COMPONENT;
            return true;
        }
    }

    PC_LOGERROR("Souldn't be here at removeComponent")
    return false;
}

bool Scene::HadComponent(Entity _entity, uint32_t _componentId, uint32_t* _outIndexinDataArray)
{
    if (!m_entities.at(_entity).isEnable)
    {
        if (_outIndexinDataArray)
        {
            *_outIndexinDataArray = NULL_COMPONENT;
        }

        return false;
    }

    if (m_entities.at(_entity).componentIdIndexInDataArray[_componentId] == NULL_COMPONENT)
    {
        if (_outIndexinDataArray)
        {
            *_outIndexinDataArray = NULL_COMPONENT;
        }

        return false;
    }

    *_outIndexinDataArray = m_entities.at(_entity).componentIdIndexInDataArray[_componentId];

    return true;
}