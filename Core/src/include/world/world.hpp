#pragma once


#include "skybox.hpp"
#include "physics/physics_wrapper.hpp";
#include "ecs/entity_manager.h"
#include "ecs/components_manager.hpp"
#include "ecs/system_manager.hpp"

BEGIN_PCCORE
class World
{
public:

    Skybox skybox;
    
    bool begin = false;

    bool run = false;

    PC_CORE_API World();
    
    PC_CORE_API ~World() = default;

    PC_CORE_API void Begin();

    PC_CORE_API void Update();

    PC_CORE_API void LoadSkyBox();

    PC_CORE_API void Destroy();

    PC_CORE_API FORCEINLINE static World* GetWorld()
    {
        return m_World;
    }



    FORCEINLINE EntityId CreateEntity()
    {
        return m_EntityManager.CreateEntity();
    }

    FORCEINLINE void RemoveEntity(EntityId _entityId)
    {
        Signature entitySignature = m_EntityManager.GetSignature(_entityId);
        m_SystemManagers.EntityDestroyed(_entityId, entitySignature);
        m_ComponentManager.DestroyEntity(_entityId, entitySignature);
        m_EntityManager.RemoveEntity(_entityId);
    }

    template <ComponentDerived T>
    void AddComponent(EntityId _entityId)
    {
        m_ComponentManager.AddComponent<T>(_entityId);

        Signature& signature = m_EntityManager.GetSignature(_entityId);
        signature.set(m_ComponentManager.GetComponentType<T>(), true);

        m_SystemManagers.EntitySignatureChanged(_entityId, signature);
    }

    template <ComponentDerived T>
    T& GetComponent(EntityId _entityId)
    {
        return m_ComponentManager.GetComponent<T>(_entityId);
    }

    template<ComponentDerived T>
    void RemoveComponent(EntityId entity)
    {
        m_ComponentManager.RemoveComponent(entity);

        Signature& signature = m_EntityManager.GetSignature(entity);
        signature.set(m_ComponentManager.GetComponentType<T>(), false);
        m_EntityManager.SetSignature(entity, signature);

        m_SystemManagers.EntitySignatureChanged(entity, signature);
    }


    template <ComponentDerived T>
    PC_FORCE_INLINE ComponentTypeBit GetComponentTypeBit()
    {
        return m_ComponentManager.GetComponentType<T>();
    }

    template<SystemDerived T>
    PC_FORCE_INLINE std::shared_ptr<T> RegisterSystem()
    {
        static_assert(!std::is_same_v<T, EcsSystem>, "EcsSystem is pure virtual ");

        return m_SystemManagers.RegisterSystem<T>();
    }


    
private:
    PC_CORE_API static inline World* m_World = nullptr;

    ComponentManager m_ComponentManager;

    EntityManager m_EntityManager;

    SystemManager m_SystemManagers;
};

END_PCCORE