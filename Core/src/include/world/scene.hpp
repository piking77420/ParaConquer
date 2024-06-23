#pragma once

#include <array>
#include <memory>
#include <vector>
#include <cassert>

#include "component.hpp"
#include "core_header.hpp"
#include "log.hpp"
#include "system.hpp"
#include "world_header_typedef.hpp"


BEGIN_PCCORE
    struct Component;

    class Scene
    {
    private:
        struct EntityInternal
        {
            bool isEnable = false;
            uint32_t* componentIdIndexInDataArray = nullptr;
            std::string name = "";
        };
    public:
        
        Entity CreateEntity();

        bool RemoveEntity(Entity _entity);

        template <typename T>
        T* AddComponent(Entity entity);

        template <typename T>
        bool RemoveComponent(Entity entity);

        template <typename T>
        T* GetComponent(Entity entity);
        
        template <typename T>
        const T* GetComponent(Entity entity) const;

        template<typename T>
        void GetComponentData(std::vector<T>** _data) const;
        
        template <typename T>
        T* AddSystem();

        template <typename T>
        T* GetSystem();

        template <typename T>
        bool RemoveSystem();

        void Update();

        void Begin();

        EntityInternal* GetEntityInternal(Entity _entity);

        const EntityInternal* GetEntityInternal(Entity _entity) const ;

        Component* AddComponentInternal(uint32_t _componentId, Entity _entity);

        bool RemoveComponentInternal(uint32_t _componentId, Entity _entity);
        
        Scene();

        ~Scene();
    
    private:
        
        std::vector<uint8_t>* m_ComponentData = nullptr;

        std::array<EntityInternal, MAX_ENTITIES> m_entities;

        std::vector<System*> systems;
        
        Component* GetComponentInternal(uint32_t _componentId, Entity _entity);

        const Component* GetComponentInternal(uint32_t _componentId, Entity _entity) const;

        bool HadComponent(Entity _entity, uint32_t _componentId, uint32_t* _outIndexinDataArray) const;
    };

    template <typename T>
    T* Scene::AddComponent(Entity _entity)
    {
        assert(ComponentHandle<T>::componentID >= 0);
        return reinterpret_cast<T*>(AddComponentInternal(ComponentHandle<T>::componentID, _entity));
    }

    template <typename T>
    bool Scene::RemoveComponent(Entity _entity)
    {
        assert(ComponentHandle<T>::componentID >= 0);
        return RemoveComponentInternal(ComponentHandle<T>::componentID, _entity);
    }

    template <typename T>
    T* Scene::GetComponent(Entity entity)
    {
        assert(ComponentHandle<T>::componentID >= 0);
        return reinterpret_cast<T*>(GetComponentInternal(ComponentHandle<T>::componentID, entity));
    }

    template <typename T>
    const T* Scene::GetComponent(Entity entity) const
    {
        assert(ComponentHandle<T>::componentID >= 0);
        return reinterpret_cast<const T*>(GetComponentInternal(ComponentHandle<T>::componentID, entity));
    }

    template <typename T>
    void Scene::GetComponentData(std::vector<T>** _data) const
    {
        assert(ComponentHandle<T>::componentID >= 0);
        *_data = reinterpret_cast< std::vector<T>*>(&m_ComponentData[ComponentHandle<T>::componentID]);
    }

    template <typename T>
    T* Scene::AddSystem()
    {
        static_assert(std::is_base_of_v<System, T>, "T is not a system");

        if (GetSystem<T>() != nullptr)
        {
            PC_LOGERROR("This scene alrealy has a " + std::to_string(typeid(T).name()));
            return nullptr;
        }

        T* newSytem = new T;
        systems.emplace_back(newSytem);

        return newSytem;
    }

    template <typename T>
    T* Scene::GetSystem()
    {
        static_assert(std::is_base_of_v<System, T>, "T is not a system");

        for (System* system : systems)
        {
            T* systemT = dynamic_cast<T>(system);
            if (systemT != nullptr)
                return system;
        }

        return nullptr;
    }

    template <typename T>
    bool Scene::RemoveSystem()
    {
        if (GetSystem<T>() == nullptr)
        {
            PC_LOGERROR("This scene alrealy desn't have " + std::to_string(typeid(T).name()));
            return false;
        }

        std::vector<System*>::iterator it = std::find_if(systems.begin(), systems.end(), [](System* system)
        {
            return dynamic_cast<T*>(system) != nullptr;
        });

        if (it != systems.end())
        {
            delete *it;
            systems.erase(it);
            return true;
        }


        return false;
    }

END_PCCORE
