#pragma once

#include "rendering/rendering_typedef.h"

BEGIN_PCCORE
 
class RenderPass
{
public:
    PC_CORE_API RenderPassHandle GetHandle() const;

    PC_CORE_API RenderPass(RenderPass&& _other) noexcept;

    PC_CORE_API RenderPass(const RenderPassCreateInfo& _renderPassCreateInfo);

    PC_CORE_API virtual void Build();
    
    PC_CORE_API RenderPass() = default;

    PC_CORE_API ~RenderPass() = default;

private:
    RenderPassHandle m_RenderPassHandle = nullptr;
    
    void Destroy();
};

END_PCCORE