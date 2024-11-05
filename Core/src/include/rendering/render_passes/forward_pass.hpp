#pragma once

#include "core_header.hpp"
#include "rendering/rendering_typedef.h"

BEGIN_PCCORE
    class ForwardPass
{
public:

    void Init();

    void Destroy();
    
    ForwardPass() = default;

    ~ForwardPass() = default;
private:
};


END_PCCORE
