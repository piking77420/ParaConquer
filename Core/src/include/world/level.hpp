#pragma once

#include "core_header.hpp"
#include "ecs/components_manager.hpp"
#include "ecs/entity_manager.h"
#include "ecs/system_manager.hpp"

BEGIN_PCCORE
    class Level : public ISeriazable
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(Level)

    PC_CORE_API Level();

    PC_CORE_API ~Level() override = default;

    IMP_DYNAMIC_REFLECT()
    
    FORCEINLINE EntityId CreateEntity(std::string&& name)
    {
        return m_EntityManager.CreateEntity(name);
    }

    FORCEINLINE EntityId CreateEntity(const std::string& name)
    {
        return m_EntityManager.CreateEntity(name);
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
        signature.set(m_ComponentManager.GetComponentTypeBit<T>(), true);

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
        m_ComponentManager.RemoveComponent(entity, Reflector::GetTypeKey<T>());

        Signature& signature = m_EntityManager.GetSignature(entity);
        signature.set(m_ComponentManager.GetComponentTypeBit<T>(), false);
        m_EntityManager.SetSignature(entity, signature);

        m_SystemManagers.EntitySignatureChanged(entity, signature);
    }


    template <ComponentDerived T>
    PC_FORCE_INLINE ComponentTypeBit GetComponentTypeBit()
    {
        return m_ComponentManager.GetComponentTypeBit<T>();
    }

    template<SystemDerived T>
    PC_FORCE_INLINE std::shared_ptr<T> RegisterSystem()
    {
        static_assert(!std::is_same_v<T, EcsSystem>, "EcsSystem is pure virtual ");

        return m_SystemManagers.RegisterSystem<T>();
    }


    //internal
    void RemoveComponentInteral(EntityId entity, TypeId typeId)
    {
        m_ComponentManager.RemoveComponent(entity, typeId);

        Signature& signature = m_EntityManager.GetSignature(entity);
        signature.set(m_ComponentManager.GetComponentTypeBit(typeId), false);
        m_EntityManager.SetSignature(entity, signature);

        m_SystemManagers.EntitySignatureChanged(entity, signature);
    }
    

private:
    ComponentManager m_ComponentManager;

    EntityManager m_EntityManager;

    SystemManager m_SystemManagers;

  

    REFLECT(Level)
    REFLECT_MEMBER(Level, m_ComponentManager)
    REFLECT_MEMBER(Level, m_EntityManager)
    REFLECT_MEMBER(Level, m_SystemManagers)
};

END_PCCORE