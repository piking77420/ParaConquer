#pragma once

#include "world_header_typedef.hpp"


BEGIN_PCCORE
class Scene;

class System
{
public:
    
    System() = default;
    
    virtual ~System() = 0;

    virtual void Begin(Scene* scene) = 0;

    virtual void Update(Scene* scene) = 0;
};

END_PCCORE
