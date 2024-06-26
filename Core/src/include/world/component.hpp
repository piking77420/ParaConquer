#pragma once

#include "core_header.hpp"
#include "world_header_typedef.hpp"
#include "component_register.h"

BEGIN_PCCORE

struct Component
{
    Entity entityID {NULL_ENTITY};
};

template<typename T>
struct ComponentHandle : public Component
{
    static inline uint32_t componentID {ComponentRegister::template RegisterComponent<T>(ComponentRegister::template CreateComponent<T>, ComponentRegister::template FreeComponent<T>)};

};

bool inline IsValid(Component component)
{
    return component.entityID != NULL_ENTITY;
}

END_PCCORE

#define MAKE_COMPONENT(x) \
PC_CORE::ComponentHandle<x> componentHolder;


