#pragma once

#include <vector>

#include "ecs_header.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
    struct EcsComponent
{
    size_t size;
    uint32_t key;
};


class EcsContext
{
public:
    
    static bool RegisterComponent(const EcsComponent& _ecsComponent);

    template <typename T>
    static bool RegisterComponent();
    
    EcsContext() = delete;

    ~EcsContext() = delete;

    static const std::vector<EcsComponent>& GetComponentsDataInfo();
    
private:
    static inline std::vector<EcsComponent> m_ECSComponentsDataInfo;
};

template <typename T>
bool EcsContext::RegisterComponent()
{
    const EcsComponent ecsComponent =
        {
        .size = sizeof(T),
        .key = Reflector::GetKey<T>() 
        };
    
    RegisterComponent(ecsComponent);

    return true;
}


END_PCCORE

