#pragma once

#include <array>
#include <memory>
#include <vector>
#include <cassert>

#include "core_header.hpp"
#include "log.hpp"
#include "system.hpp"
#include "ecs/entity_register.h"
#include "reflection/reflector.hpp"


BEGIN_PCCORE
    struct Entity
    {
        std::string name;
        EntityId ecsId;
    };
/*
    REFLECT(Entity)
    REFLECT_MEMBER(Entity, name)
    REFLECT_MEMBER(Entity, ecsId)
  */  
    
    class Scene
    {
    public:
        PC_CORE_API void Begin();
        
        PC_CORE_API void Update();

        PC_CORE_API Scene() = default;
        
        PC_CORE_API ~Scene() = default;

        PC_CORE_API EntityId CreateEntity(const std::string& name);

        PC_CORE_API const Entity* GetEntity(EntityId _id) const;

        PC_CORE_API Entity* GetEntity(EntityId _id);
    
        template <class T>
        T* Get(EntityId _entityId);
    
        template<typename T>
        const T* Get(EntityId _entityId) const;

        PC_CORE_API void* Get(EntityId _entityId, uint32_t _componentKey);
        
        template<typename T>
        T* AddComponent(EntityId _entityId);

        PC_CORE_API void AddComponent(EntityId _entityId, uint32_t _componentKey);
    
        template<typename T>
        void RemoveComponent(EntityId _entityId);

        PC_CORE_API void RemoveComponent(EntityId _entityId, uint32_t _componentKey);

        template<typename T>
        bool HasComponent(EntityId _entityId) const;

        PC_CORE_API bool HasComponent(EntityId _entityId, uint32_t _componentKey) const;
    
        template<typename T>
        std::vector<T>* GetData();

        template<typename T>
        const std::vector<T>* GetData() const;
        
        // TODO make it private
        EntityRegister m_EntityRegister;
        
        std::vector<Entity> m_Entities; 
    private:
    };

    template <typename T>
    T* Scene::Get(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<T*>(m_EntityRegister.GetComponent(_entityId, key));
    }

    template <typename T>
    const T* Scene::Get(EntityId _entityId) const
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<const T*>(m_EntityRegister.GetComponent(_entityId, key));
    }

    template <typename T>
    T* Scene::AddComponent(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<T*>(m_EntityRegister.CreateComponent(_entityId, key));
    }

   

    template <typename T>
    void Scene::RemoveComponent(EntityId _entityId)
    {
        RemoveComponent(_entityId, Reflector::GetKey<T>());
    }

    template <typename T>
    bool Scene::HasComponent(EntityId _entityId) const
    {
        return m_EntityRegister.IsEntityHasComponent(_entityId, Reflector::GetKey<T>());
    }

    template <typename T>
    std::vector<T>* Scene::GetData()
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<std::vector<T>*>(m_EntityRegister.GetComponentData(key));
    }

    template <typename T>
    const std::vector<T>* Scene::GetData() const
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<const std::vector<T>*>(m_EntityRegister.GetComponentData(key));
    }

END_PCCORE
