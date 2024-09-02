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
    class Scene
    {
    public:
        void Begin();
        
        void Update();

        Scene() = default;
        
        ~Scene() = default;
    
        template <class T>
        T& Get(EntityId _entityId);
    
        template<typename T>
        const T& Get(EntityId _entityId) const;

        template<typename T>
        const T& Create(EntityId _entityId);
    
        template<typename T>
        const T& Delete(EntityId _entityId);

        template<typename T>
        uint8_t* GetData();

    // TO DO make it private
        EntityRegister m_EntityRegister;
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
    const T& Scene::Create(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return m_EntityRegister.CreateComponent(_entityId, key);
    }

    template <typename T>
    const T& Scene::Delete(EntityId _entityId)
    {
        const uint32_t key = Reflector::GetKey<T>();
        return m_EntityRegister.DeleteComponent(_entityId, key);
    }

    template <typename T>
    uint8_t* Scene::GetData()
    {
        const uint32_t key = Reflector::GetKey<T>();
        return m_EntityRegister.GetComponentData(key);
    }

  
END_PCCORE
