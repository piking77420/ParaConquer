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

        Entity() = default;

        ~Entity() = default;

        DEFAULT_COPY_MOVE_OPERATIONS(Entity)
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

        DEFAULT_COPY_MOVE_OPERATIONS(Scene)

        PC_CORE_API Scene() = default;
        
        PC_CORE_API ~Scene() = default;

        PC_CORE_API Entity* CreateEntity(const std::string& name);

        PC_CORE_API const Entity* GetEntityFromId(EntityId _entityId) const;

        PC_CORE_API Entity* GetEntityFromId(EntityId _entityId);
    
        template <class T>
        T* GetComponent(Entity* _entity);
    
        template<typename T>
        const T* GetComponent(const Entity* _entity) const;

        PC_CORE_API void* GetComponent(Entity* _entity, uint32_t _componentKey);

        template<typename T>
        T* AddComponent(Entity* _entity);

        PC_CORE_API void AddComponent(Entity* _entity, uint32_t _componentKey);

        template<typename T>
        void RemoveComponent(Entity* _entity);

        PC_CORE_API void RemoveComponent(Entity* _entity, uint32_t _componentKey);

        template<typename T>
        bool HasComponent(Entity* _entity) const;

        PC_CORE_API bool HasComponent(Entity* _entity, uint32_t _componentKey) const;
    
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
    T* Scene::GetComponent(Entity* _entity)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<T*>(m_EntityRegister.GetComponent(_entity->ecsId, key));
    }

    template <typename T>
    const T* Scene::GetComponent(const Entity* _entity) const
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<const T*>(m_EntityRegister.GetComponent(_entity->ecsId, key));
    }

    template <typename T>
    T* Scene::AddComponent(Entity* _entity)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return reinterpret_cast<T*>(m_EntityRegister.CreateComponent(_entity->ecsId, key));
    }



    template <typename T>
    void Scene::RemoveComponent(Entity* _entity)
    {
        RemoveComponent(_entity, Reflector::GetKey<T>());
    }

    template <typename T>
    bool Scene::HasComponent(Entity* _entity) const
    {
        return m_EntityRegister.IsEntityHasComponent(_entity->ecsId, Reflector::GetKey<T>());
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
