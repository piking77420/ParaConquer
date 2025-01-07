#pragma once

#include "component.h"
#include "ecs_header.h"

BEGIN_PCCORE


class EcsArchetype
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(EcsArchetype)
    
    EcsArchetype() = default;


    

private:
    std::vector<ReflectedType*> m_types;
};

END_PCCORE