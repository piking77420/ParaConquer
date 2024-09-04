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
    EcsContext();

    ~EcsContext();
    
    static const std::vector<EcsComponent>& GetComponentsDataInfo();
    
private:
    static inline std::vector<EcsComponent> m_ECSComponentsDataInfo;
};

END_PCCORE

