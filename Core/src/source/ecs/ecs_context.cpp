#include "ecs/ecs_context.h"

bool PC_CORE::EcsContext::RegisterComponent(const EcsComponent& _ecsComponent)
{
    m_ECSComponentsDataInfo.push_back(_ecsComponent);
    return true;
}

const std::vector<PC_CORE::EcsComponent>& PC_CORE::EcsContext::GetComponentsDataInfo()
{
    return m_ECSComponentsDataInfo;
}
