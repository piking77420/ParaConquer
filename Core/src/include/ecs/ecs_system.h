#pragma once

#include <set>

#include "ecs_header.h"

BEGIN_PCCORE


class EcsSystem
{
public:
    PC_CORE_API EcsSystem() = default;

    PC_CORE_API virtual ~EcsSystem() = default;

    PC_CORE_API virtual void Begin() = 0;

    PC_CORE_API virtual void Tick(double deltaTime) = 0;
    
    PC_CORE_API virtual void RenderingTick(double deltatime) = 0;


    PC_CORE_API void OnEntityDestroy(EntityId id, Signature _signature)
    {
        auto it = m_SignatureEntitiesSet.find(_signature);

        if (it == m_SignatureEntitiesSet.end())
            return;

        it->second.erase(id)    ;
    }

    PC_CORE_API PC_FORCE_INLINE bool OnEntitySignatureChange(EntityId id, Signature _signature)
    {
        auto it = m_SignatureEntitiesSet.find(_signature);
   
        if (it == m_SignatureEntitiesSet.end())
            return false;
        
        it->second.insert(id);
        return true;
    }

    PC_CORE_API PC_FORCE_INLINE void AddSignature(Signature _signature)
    {
        m_SignatureEntitiesSet.insert({ _signature,{} });
    }

    
public:
    std::unordered_map<Signature, std::set<EntityId>> m_SignatureEntitiesSet;

};

template<class T>
concept SystemDerived = std::is_base_of_v<EcsSystem, T>;


REFLECT(EcsSystem)


END_PCCORE