#pragma once

#include <array>
#include <string>
#include <vector>

#include "component.h"
#include "ecs_system.h"
#include "core_header.hpp"
#include "ecs_header.h"
#include "ecs_system.h"
#include "data_structures/type_sparse_set.hpp"

BEGIN_PCCORE

    template<class T>
    concept SystemDerived = std::is_base_of_v<Component, T>;

    struct Entity
    {
        EntityId id = INVALID_ENTITY_ID;
    
        std::string name;

        DEFAULT_COPY_MOVE_OPERATIONS(Entity)
    
        Entity() = default;

        ~Entity() = default;

        Entity(EntityId _id, std::string&& _name) : id(_id), name(std::move(_name))
        {
        }
    };

    /*
        REFLECT(Entity)
        REFLECT_MEMBER(Entity, name)
        REFLECT_MEMBER(Entity, ecsId)
      */
    class EntityManager
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(EntityManager)
        
        PC_CORE_API EntityManager();

        PC_CORE_API ~EntityManager() = default;

        inline uint32_t GetEntityCount() const
        {
            return m_EntityCount;
        }

        inline uint32_t GetEntityCapacity() const
        {
            return static_cast<uint32_t>(m_Entities.size());
        }
        
        PC_CORE_API EntityId CreateEntity();

        PC_CORE_API EntityId CreateEntity(std::string&& name);
        
        PC_CORE_API bool CreateEntity(EntityId _id, const std::string& name);

        PC_CORE_API void DestroyEntity(const Entity& _entity);

        PC_CORE_API Entity& GetEntity(EntityId _entityId);

        PC_CORE_API const Entity& GetEntity(EntityId _entityId) const;

        PC_CORE_API inline uint32_t GetEntitiesCount() const
        {
            return m_EntityCount;
        }

        PC_CORE_API Component* AddComponent(uint32_t typeKey,const EntityId& _entityId);

        template<class T>
        T* AddComponent(const EntityId& _entityId);
        
        PC_CORE_API void RemoveComponent(uint32_t typeKey, const EntityId& _entityId);
        
        template <class T>
        void RemoveComponent(const EntityId& _entityId);

        template <class T>
        const T* GetComponent(const EntityId& _entityId) const;
        
        template <class T>
        T* GetComponent(const EntityId& _entityId);

        PC_FORCE_INLINE Component* GetComponent(uint32_t _componentKey ,const EntityId& _entityId)
        {
            TypeSparseSet& sparseSet = m_SparseSetsMap.at(_componentKey);
            return reinterpret_cast<Component*>(&sparseSet[_entityId]);
        }


        template <class ...T>
        void GetComponents(const EntityId& _entityId, std::tuple<const T&...>& components) const;
        
        template <class ...T>
        void GetComponents(const EntityId& _entityId, std::tuple<T&...>& components);

        template <class T>
        bool HasComponent(const EntityId& _entityId) const;
        
        PC_FORCE_INLINE bool HasComponent(uint32_t _key, const EntityId& _entity) const
        {
            return  m_SparseSetsMap.at(_key).IsValid(_entity);
        }
        
        template <class ...T>
        bool HasComponents(const EntityId& _entityId) const;

        PC_CORE_API std::string GetEntityName(EntityId _entityId) const;

        PC_CORE_API std::string& GetEntityName(EntityId _entityId);

        PC_CORE_API void SetEntityName(EntityId _entityId, const std::string& _name);
        
        template <ComponentDerived... C, typename... Args>
        void ForEach(std::function<void(C&..., Args...)> _func, Args&&... _args);

        template <ComponentDerived... C, typename... Args>
        void ForEach(std::function<void(const C&..., Args...)> _func, Args&&... _args) const;

        template <ComponentDerived... C>
        void ForEach(std::function<void(C&...)> _func);

        template <ComponentDerived... C>
        void ForEach(std::function<void(const C&...)> _func) const;

        template <SystemDerived ...T>
        void AddSystem()
        {
            m_EcsSystems.reserve(sizeof...(T));

            m_EcsSystems.emplace_back(new T()...);
        }

        template <SystemDerived ...T>
        void RemoveSystem()
        {
         
        }

        
        inline bool IsValidEntityId(EntityId _entityId) const
        {
            if (m_Entities.size() <= _entityId)
                return false;

            if (m_Entities[_entityId].id == INVALID_ENTITY_ID)
                return false;

            return true;
        }

    private:

        uint32_t m_EntityCount = 0;
        
        std::vector<Entity> m_Entities;
        
        std::unordered_map<uint32_t, TypeSparseSet> m_SparseSetsMap;

        std::vector<EcsSystem*> m_EcsSystems;
       
    };

    
    /* Exemple
    std::function<void(Transform&, RigidBody&, int, float)> d = FuncParam;
    World::GetWorld()->entityManager.ForEach<Transform, RigidBody>(d, 20, 26.f);

    std::function<void(Transform&, RigidBody&)> c = Func;
    World::GetWorld()->entityManager.ForEach<Transform, RigidBody>(c);

    // MEMEBER

    c = std::bind(&Editor::FuncMember, this, std::placeholders::_1, std::placeholders::_2);
    World::GetWorld()->entityManager.ForEach<Transform, RigidBody>(c);
    */

    template <class T>
    T* EntityManager::AddComponent(const EntityId& _entity)
    {
        static_assert(std::is_base_of_v<Component, T>,"T is not a Component");
        constexpr uint32_t key = Reflector::GetTypeKey<T>();
        TypeSparseSet& sparseSet = m_SparseSetsMap.at(key);
        
        T* ptr = reinterpret_cast<T*>(sparseSet.Create(_entity));

        if (ptr == nullptr)
            return ptr;
        
        ptr->entityId = _entity;
        ptr = new (ptr) T();
        
        return ptr;
    }

    template <class T>
    void EntityManager::RemoveComponent(const EntityId& _entityId)
    {
        static_assert(std::is_base_of_v<Component, T >,"T is not a Component");

        TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetTypeKey<T>());

        T* component = GetComponent<T>();
        component->~T();
        
        sparseSet.Free(_entityId);
    }

    template <class T>
    const T* EntityManager::GetComponent(const EntityId& _entity) const
    {
        //static_assert(!std::is_base_of_v<T, Component> ,"T is not a Component");

        TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetTypeKey<T>());
        return reinterpret_cast<const T*>(&sparseSet[_entity]);

        return nullptr;
    }

    template <class T>
    T* EntityManager::GetComponent(const EntityId& _entity)
    {
        TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetTypeKey<T>());
        return reinterpret_cast<T*>(&sparseSet[_entity]);
    }

    template <class ... T>
    void EntityManager::GetComponents(const EntityId& _entity, std::tuple<const T&...>& components) const
    {
        (..., (std::get<const T&>(components) = *GetComponent<T>(_entity)));
    }

    template <class... T>
    void EntityManager::GetComponents(const EntityId& _entity, std::tuple<T&...>& components)
    {
        (..., (std::get<T&>(components) = *GetComponent<T>(_entity)));
    }

    template <class T>
    bool EntityManager::HasComponent(const EntityId& _entity) const
    {
        return  m_SparseSetsMap.at(Reflector::GetTypeKey<T>()).IsValid(_entity);
    }


    template <class ...T>
    bool EntityManager::HasComponents(const EntityId& _entity) const
    {
        return (HasComponent<T>(_entity) && ...);
    }


template <ComponentDerived ... C ,typename ...Args>
void EntityManager::ForEach(std::function<void(const C&..., Args...)> _func, Args&&... _args) const
{
        for (auto& entity : m_Entities)
        {
            if (entity.id == INVALID_ENTITY_ID)
                continue;

            if (!HasComponents<C...>(entity.id))
                continue;

            std::tuple<const C&...> components(*GetComponent<C>(entity.id)...);


            std::apply(_func, std::tuple_cat(components, std::forward_as_tuple<Args>(_args)...));
        }
}

template <ComponentDerived ... C>
void EntityManager::ForEach(std::function<void(C&...)> _func)
{
        for (auto& entity : m_Entities)
        {
            if (entity.id == INVALID_ENTITY_ID)
                continue;

            if (!HasComponents<C...>(entity.id))
                continue;

            std::tuple<C&...> components(*GetComponent<C>(entity.id)...);
            
            std::apply(
                _func,
                components);
        }
}

template <ComponentDerived ... C>
void EntityManager::ForEach(std::function<void(const C&...)> _func) const
{
        for (auto& entity : m_Entities)
        {
            if (entity.id == INVALID_ENTITY_ID)
                continue;

            if (!HasComponents<C...>(entity.id))
                continue;

            std::tuple<const C&...> components(*GetComponent<C>(entity.id)...);
            
            std::apply(
                _func,
                components);
        }
}


template <ComponentDerived ... C, typename ...Args>
void EntityManager::ForEach(std::function<void(C&..., Args...)> _func, Args&&... _args)
{
        for (auto& entity : m_Entities)
        {
            if (entity.id == INVALID_ENTITY_ID)
                continue;

            if (!HasComponents<C...>(entity.id))
                continue;

            std::tuple<C&...> components(*GetComponent<C>(entity.id)...);


            std::apply(
           _func, 
           std::tuple_cat(components, std::forward_as_tuple(std::forward<Args>(_args)...)));
        }
}





END_PCCORE
