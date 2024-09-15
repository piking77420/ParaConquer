#pragma once

#include "core_header.hpp"

BEGIN_PCCORE
    
    class CoreIo
    {
    public:
        CoreIo() = delete;

        ~CoreIo() = delete;

        PC_CORE_API static void Init();

        PC_CORE_API static void Destroy();

        PC_CORE_API static void PoolEvent();

        
        
    private:
        
    };
END_PCCORE
