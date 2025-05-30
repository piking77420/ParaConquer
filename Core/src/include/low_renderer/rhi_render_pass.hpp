#pragma once

#include <vector>

#include "rhi_typedef.h"

BEGIN_PCCORE
    struct RenderPassAttachementDescriptor
{
    TextureAttachement attachementType;
    RHIFormat format;
    bool readOnly = false;
    
    int sampleCount;
    PC_CORE::LoadOperation load;
    PC_CORE::StoreOperation store;

    PC_CORE::LoadOperation stencilLoad;
    PC_CORE::StoreOperation stencilStore;
};

struct RenderPassDescriptor
{
    ShaderProgramPipelineType ShaderProgramPipelineType; 
    std::vector<RenderPassAttachementDescriptor> attachements;
};


class RhiRenderPass
{
public:
    
    PC_CORE_API RhiRenderPass(PC_CORE::RHIFormat colorFormat, PC_CORE::RHIFormat depthFormat) {}
    
    PC_CORE_API RhiRenderPass() = default;

    PC_CORE_API RhiRenderPass(const RenderPassDescriptor& _attachementDescriptors) {}
    
    PC_CORE_API virtual ~RhiRenderPass() = default;
    
protected:
};

END_PCCORE
