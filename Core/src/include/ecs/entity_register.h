#pragma once

#include <array>
#include <string>
#include <vector>

#include "component.h"
#include "core_header.hpp"
#include "ecs_header.h"
#include "data_structures/type_sparse_set.hpp"

BEGIN_PCCORE
    struct Entity
    {
        EntityId id = INVALID_ENTITY_ID;
    
        std::string name;

        Entity() = default;

        ~Entity() = default;

        DEFAULT_COPY_MOVE_OPERATIONS(Entity)
    };

    /*
        REFLECT(Entity)
        REFLECT_MEMBER(Entity, name)
        REFLECT_MEMBER(Entity, ecsId)
      */
    class EntityRegister
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(EntityRegister)
        
        PC_CORE_API EntityRegister();

        PC_CORE_API ~EntityRegister() = default;
        
        PC_CORE_API const Entity& CreateEntity() const;

        PC_CORE_API Entity& CreateEntity();

        PC_CORE_API void DestroyEntity(const Entity& _entity);

        template<class T>
        T* AddComponent(const Entity& _entity);
        
        template <class T>
        void RemoveComponent(const Entity& _entity);

        template <class T>
        const T* GetComponent(const Entity& _entity) const;
        
        template <class T>
        T* GetComponent(const Entity& _entity);

        template <class T>
        bool HasComponent(const Entity& _entity) const;
        


        /*
        template <class ... C>
        void ForEach(const std::function<void(const C&...)>& _callback);*/
    
        mutable std::array<Entity, MAX_ENTITIES> entities;

    private:

        std::unordered_map<uint32_t, TypeSparseSet> m_SparseSetsMap;
    };

    template <class T>
    T* EntityRegister::AddComponent(const Entity& _entity)
    {
        static_assert(std::is_base_of_v<Component, T>,"T is not a Component");
        
        TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetKey<T>());
        return reinterpret_cast<T*>(sparseSet.Create(_entity.id));
    }

    template <class T>
    void EntityRegister::RemoveComponent(const Entity& _entity)
    {
        static_assert(std::is_base_of_v<Component, T >,"T is not a Component");

        TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetKey<T>());

        sparseSet.Free(_entity.id);
    }

    template <class T>
    const T* EntityRegister::GetComponent(const Entity& _entity) const
    {
        //static_assert(!std::is_base_of_v<T, Component> ,"T is not a Component");

        const TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetKey<T>());

        try
        {
            if (sparseSet.IsValid(_entity.id))
            {
                return reinterpret_cast<const T*>(&sparseSet[_entity.id]);
            }
        }
        catch (...)
        {
            PC_LOGERROR("EntityRegister::GetComponent failed");
            return nullptr;
        }

        return nullptr;
    }

    template <class T>
    T* EntityRegister::GetComponent(const Entity& _entity)
    {
        TypeSparseSet& sparseSet = m_SparseSetsMap.at(Reflector::GetKey<T>());

        try
        {
            if (sparseSet.IsValid(_entity.id))
                return reinterpret_cast<T*>(&sparseSet[_entity.id]);
        }
        catch (...)
        {
            PC_LOGERROR("EntityRegister::GetComponent failed");
            return nullptr;
        }

        return nullptr;
    }

    template <class T>
    bool EntityRegister::HasComponent(const Entity& _entity) const
    {
        return m_SparseSetsMap.at(Reflector::GetKey<T>()).IsValid(_entity.id);
    }


END_PCCORE
