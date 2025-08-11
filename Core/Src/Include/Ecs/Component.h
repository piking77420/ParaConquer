#pragma once

#include "CoreHeader.hpp"
#include "EcsFront.h"



BEGIN_PCCORE

using ComponentTypeBit = uint32_t;

struct Component
{
    PC_FORCE_INLINE EntityId GetEntityId() const
    {
        return entityId;
    }

    bool operator==(const Component&) const = default;

    bool operator!=(const Component&) const = default;

private:
    // DO NOT INIT THIS VALUE OR I WILL KILL YOU
    // THIS entityId IS SET BEFORE THE CONSTRUCTOR 
    // IN ORDER TO HAVE THE ENTITY ID AT CONTRUCTOR
    EntityId entityId;

    static_assert(sizeof(Component::entityId) == sizeof(EntityId), "Entity Id must be alone and first");

    REFLECT(Component);
    REFLECT_MEMBER(Component, entityId);
};
template<class T>
   concept ComponentDerived = std::is_base_of_v<Component, T>;


END_PCCORE