#pragma once

#include <set>

#include "ecs_header.h"

BEGIN_PCCORE


class EcsSystem : public DynamicReflectable
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(EcsSystem)
    
    PC_CORE_API EcsSystem() = default;

    PC_CORE_API virtual ~EcsSystem() = default;

    PC_CORE_API virtual void Begin() = 0;

    PC_CORE_API virtual void Tick(double deltaTime) = 0;
    
    PC_CORE_API virtual void RenderingTick(double deltatime) = 0;


    PC_CORE_API void OnEntityDestroy(EntityId id, Signature _signature)
    {
        auto it = GetEntitySet(_signature);

        if (it == nullptr)
            return;

        it->erase(id)    ;
    }

    PC_CORE_API PC_FORCE_INLINE void OnEntitySignatureChange(EntityId id, const Signature& _old, const Signature& _new)
    {
        {
            // delete
            std::set<EntityId>* it = GetEntitySet(_old);
            if (it != nullptr)
                it->erase(id);
        }

        {
            // add
            std::set<EntityId>* it = GetEntitySet(_new);
            if (it != nullptr)
                it->emplace(id);
        }

    }

    PC_CORE_API PC_FORCE_INLINE void AddSignature(const Signature& _signature)
    {
        m_SignatureEntitiesSet.emplace_back(_signature, std::set<EntityId>());
    }

    const std::set<EntityId>* GetEntityIdList(const Signature& _signature) const
    {
        for (auto& it : m_SignatureEntitiesSet)
        {
            if (it.first == _signature)
                return &it.second;
        }

        return nullptr;
    }
    
protected:
    std::vector<std::pair<Signature, std::set<EntityId>>> m_SignatureEntitiesSet;

    std::set<EntityId>* GetEntitySet(const Signature& _signature)
    {
        for (auto& it : m_SignatureEntitiesSet)
        {
            if (it.first == _signature)
                return &it.second;
        }

        return nullptr;
    }

};

template<class T>
concept SystemDerived = std::is_base_of_v<EcsSystem, T>;


REFLECT(EcsSystem)


END_PCCORE