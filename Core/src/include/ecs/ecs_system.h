#pragma once

#include "ecs_header.h"

BEGIN_PCCORE

class EcsSystem
{
public:
    
    PC_CORE_API EcsSystem() = default;

    PC_CORE_API virtual ~EcsSystem() = default;

    PC_CORE_API virtual void Begin() = 0;

    PC_CORE_API virtual void Tick(double deltaTime) = 0;
    
    
private:
    
    
};

REFLECT(EcsSystem)


END_PCCORE