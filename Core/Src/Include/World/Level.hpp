#pragma once

#include "CoreHeader.hpp"
#include "Ecs/ComponentsManager.hpp"
#include "Ecs/EntityManager.h"
#include "Ecs/SystemManager.hpp"

BEGIN_PCCORE
    class Level : public ISeriazable
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(Level)

    PC_CORE_API Level();

    PC_CORE_API ~Level() override = default;

    IMP_DYNAMIC_REFLECT()

    PC_FORCE_INLINE EntityId CreateEntity()
    {
        return m_EntityManager.CreateEntity();
    }
    
    PC_FORCE_INLINE EntityId CreateEntity(std::string&& name)
    {
        return m_EntityManager.CreateEntity(name);
    }

    PC_FORCE_INLINE EntityId CreateEntity(const std::string& name)
    {
        return m_EntityManager.CreateEntity(name);
    }

    PC_FORCE_INLINE void RemoveEntity(EntityId _entityId)
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
        UpdateSignature(entity, Reflector::GetTypeKey<T>(), false);
    }


    template <ComponentDerived T>
    PC_FORCE_INLINE ComponentTypeBit GetComponentTypeBit()
    {
        return m_ComponentManager.GetComponentTypeBit<T>();
    }

    template<SystemDerived T, typename... P>
    PC_FORCE_INLINE std::shared_ptr<T> RegisterSystem(P&&... args)
    {
        static_assert(!std::is_same_v<T, EcsSystem>, "EcsSystem is pure virtual");

        return m_SystemManagers.RegisterSystem<T>(std::forward<P>(args)...);
    }

    template<SystemDerived T>
    PC_FORCE_INLINE std::shared_ptr<T> RegisterSystem()
    {
        static_assert(!std::is_same_v<T, EcsSystem>, "EcsSystem is pure virtual");

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
    
    PC_CORE_API PC_FORCE_INLINE bool IsValid(EntityId _id) const
    {
        return m_EntityManager.IsValid(_id);
    }

    PC_CORE_API Signature* GetSignature(EntityId entity)
    {
        return m_EntityManager.GetSignature(entity);
    }

    PC_CORE_API const Signature* GetSignature(EntityId entity) const
    {
        return m_EntityManager.GetSignature(entity);
    }

    template <ComponentDerived... T>
    PC_FORCE_INLINE bool HasComponent(EntityId _entityId)
    {
        auto signature = m_EntityManager.GetSignature(_entityId);
        return (signature->test(GetComponentTypeBit<T>()) && ...);
    }

    std::string_view GetEntityName(EntityId _entityId) const
    {
        return m_EntityManager.GetEntityName(_entityId);
    }

    PC_CORE_API void Begin()
    {
        m_SystemManagers.Begin();
    }

    PC_CORE_API void Update(double _tick)
    {
        m_SystemManagers.Update(_tick);
    }


    PC_CORE_API void RenderingTick(double _tick)
    {
        m_SystemManagers.RenderingTick(_tick);
    }
private:
    ComponentManager m_ComponentManager;

    EntityManager m_EntityManager;

    SystemManager m_SystemManagers;

    void UpdateSignature(EntityId entity, TypeId typeId, bool hasComponent)
    {
        PERF_REGION_SCOPED;
        
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