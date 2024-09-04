#include "ecs/ecs_context.h"
#include "ecs/ecs_front.h"

namespace PC_CORE
{
    struct Component;
}

PC_CORE::EcsContext::EcsContext()
{
    auto vec = Reflector::GetAllTypesFrom<Component>();
    
    for (auto& component : vec)
    {
        const EcsComponent ecsComponent =
            {
            .size = component->dataSize,
            .key = component->HashKey,
            .Createfunc = component->createFunc,
            .Deletefunc = component->deleteFunc,
            };
        
        m_ECSComponentsDataInfo.emplace_back(ecsComponent);
    }
}

PC_CORE::EcsContext::~EcsContext()
{
}

const std::vector<PC_CORE::EcsComponent>& PC_CORE::EcsContext::GetComponentsDataInfo()
{
    return m_ECSComponentsDataInfo;
}
