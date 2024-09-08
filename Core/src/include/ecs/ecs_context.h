#pragma once

#include <vector>

#include "ecs_header.h"
#include "sparse_sets.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
struct EcsComponent
{
    size_t size;
    uint32_t key;
    SparsetCreateFunc Createfunc;
    SparsetDeleteFunc Deletefunc;
};


class EcsContext
{
public:
    PC_CORE_API EcsContext();

    PC_CORE_API ~EcsContext();
    
    PC_CORE_API static const std::vector<EcsComponent>& GetComponentsDataInfo();
    
private:
    PC_CORE_API static inline std::vector<EcsComponent> m_ECSComponentsDataInfo;
};

END_PCCORE

