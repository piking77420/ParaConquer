#pragma once

#include "core_header.hpp"

BEGIN_PCCORE
    
    class CoreIo
    {
    public:
        PC_CORE_API CoreIo();

        PC_CORE_API ~CoreIo();

        PC_CORE_API void PoolEvent();
    };
END_PCCORE
