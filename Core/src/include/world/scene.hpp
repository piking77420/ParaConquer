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
    
    class Scene
    {
    public:
        void Begin();
        
        void Update();

        Scene() = default;
        
        ~Scene() = default;

        EntityId CreateEntity(const std::string& name);

        const Entity* GetEntity(EntityId _id) const;

        Entity* GetEntity(EntityId _id);
    
        template <class T>
        T& Get(EntityId _entityId);
    
        template<typename T>
        const T& Get(EntityId _entityId) const;
        
        template<typename T>
        void AddComponent(EntityId _entityId);
    
        template<typename T>
        void RemoveComponent(EntityId _entityId);
    
        template<typename T>
        uint8_t* GetData(size_t* _bufferSizeT);

        template<typename T>
        const uint8_t* GetData(size_t* _bufferSizeT) const;

        // ReturnComponent Data from a component key and _bufferSizeT is the nbr of component
        uint8_t* GetData(uint32_t _componentKey, size_t _componentSize, size_t* _bufferSizeT);

        // TODO make it private
        EntityRegister m_EntityRegister;
        
        std::vector<Entity> m_Entities; 
    private:
    };

    template <typename T>
    T& Scene::Get(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return *reinterpret_cast<T*>(m_EntityRegister.GetComponent(_entityId, key));
    }

    template <typename T>
    const T& Scene::Get(EntityId _entityId) const
    {
        const uint32_t key = Reflector::GetKey<T>();
        return *reinterpret_cast<const T*>(m_EntityRegister.GetComponent(_entityId, key));
    }

    template <typename T>
    void Scene::AddComponent(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        m_EntityRegister.CreateComponent(_entityId, key);
    }

    template <typename T>
    void Scene::RemoveComponent(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        m_EntityRegister.DeleteComponent(_entityId, key);
    }

    // Return 
    template <typename T>
    uint8_t* Scene::GetData(size_t* _bufferSizeT)
    {
        const uint32_t key = Reflector::GetKey<T>();
        size_t dataSize = 0; 
        uint8_t* data = m_EntityRegister.GetComponentData(key, &dataSize);
        *_bufferSizeT = dataSize / sizeof(T);
        return data;
    }

    template <typename T>
    const uint8_t* Scene::GetData(size_t* _bufferSizeT) const
    {
        const uint32_t key = Reflector::GetKey<T>();
        size_t dataSize = 0; 
        const uint8_t* data = m_EntityRegister.GetComponentData(key, &dataSize);
        *_bufferSizeT = dataSize / sizeof(T);
        return data;
    }


END_PCCORE
