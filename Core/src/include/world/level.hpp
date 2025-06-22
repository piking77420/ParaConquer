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

    FORCEINLINE EntityId CreateEntity()
    {
        return m_EntityManager.CreateEntity();
    }
    
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
        Signature* entitySignature = m_EntityManager.GetSignature(_entityId);
        m_SystemManagers.EntityDestroyed(_entityId, *entitySignature);
        m_ComponentManager.DestroyEntity(_entityId, *entitySignature);
        m_EntityManager.RemoveEntity(_entityId);
    }

    template <ComponentDerived T>
    void AddComponent(EntityId _entityId)
    {
        UpdateSignature(_entityId, Reflector::GetTypeKey<T>(), true);
    }

    template <ComponentDerived T>
    T& GetComponent(EntityId _entityId)
    {
        return m_ComponentManager.GetComponent<T>(_entityId);
    }

    template <ComponentDerived T>
    const T& GetComponent(EntityId _entityId) const
    {
        return m_ComponentManager.GetComponent<T>(_entityId);
    }

    template<ComponentDerived T>
    void RemoveComponent(EntityId entity)
    {
        UpdateSignature(entity, Reflector::GetTypeKey<T>(), true);
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

    void AddComponent(EntityId entity, TypeId typeId)
    {
        UpdateSignature(entity, typeId, true);
    }
    
    void RemoveComponent(EntityId entity, TypeId typeId)
    {
        UpdateSignature(entity, typeId, false);
    }


    uint8_t& GetComponent(EntityId _entityId, TypeId typeId)
    {
        return m_ComponentManager.GetComponent(_entityId, typeId);
    }

    const uint8_t& GetComponent(EntityId _entityId, TypeId typeId) const
    {
        return m_ComponentManager.GetComponent(_entityId, typeId);
    }
    

private:
    ComponentManager m_ComponentManager;

    EntityManager m_EntityManager;

    SystemManager m_SystemManagers;

    void UpdateSignature(EntityId entity, TypeId typeId, bool hasComponent)
    {
        if (hasComponent)
        {
            m_ComponentManager.AddComponent(entity, typeId);
        }
        else
        {
            m_ComponentManager.RemoveComponent(entity, typeId);
        }

        Signature* signaturePtr = m_EntityManager.GetSignature(entity);
        if (signaturePtr != nullptr)
        {
            Signature oldSignature = *signaturePtr;
            Signature newSignature = oldSignature;

            newSignature.set(m_ComponentManager.GetComponentTypeBit(typeId), hasComponent);
            m_EntityManager.SetSignature(entity, newSignature);

            m_SystemManagers.EntitySignatureChanged(entity, oldSignature, newSignature);
        }
    }
    
    REFLECT(Level)
    REFLECT_MEMBER(Level, m_ComponentManager)
    REFLECT_MEMBER(Level, m_EntityManager)
    REFLECT_MEMBER(Level, m_SystemManagers)
};

END_PCCORE