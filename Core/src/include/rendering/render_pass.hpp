#pragma once

#include "rendering/rendering_typedef.h"

BEGIN_PCCORE
    enum class RenderPassTargetFlags
    {
        NONE,
        COLOR,
        DEPTH,
        STENCIL,
        COUNT,
    };
    ENUM_FLAGS(RenderPassTargetFlags)

    struct RenderPassCreateInfo
    {
        ShaderProgramPipelineType shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS;
        RenderPassTargetFlags renderPassTargetFlags = RenderPassTargetFlags::NONE;
    };
class RenderPass
{
public:
    PC_CORE_API RenderPassHandle GetHandle() const;

    PC_CORE_API RenderPass(RenderPass&& _other) noexcept;

    PC_CORE_API RenderPass(const RenderPassCreateInfo& _renderPassCreateInfo);
    
    PC_CORE_API RenderPass() = default;

    PC_CORE_API ~RenderPass() = default;
 
private:
    RenderPassHandle m_RenderPassHandle = nullptr;
};

END_PCCORE