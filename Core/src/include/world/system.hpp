#pragma once

#include "core_header.hpp"

BEGIN_PCCORE
class Scene;

class System
{
public:
    
    System() = default;
    
    PC_CORE_API virtual ~System() = 0;

    PC_CORE_API virtual void Begin(Scene* scene) = 0;

    PC_CORE_API virtual void Update(Scene* scene) = 0;
};

END_PCCORE
