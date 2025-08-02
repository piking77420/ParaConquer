#pragma once

#include <vector>

#include "rhi_typedef.h"
#include "rhi_resource.hpp"

BEGIN_PCCORE

struct RenderPassAttachementDescriptor
{
    AttachmentType attachmentType;
    RHIFormat format;
    int sampleCount;

    PC_CORE::LoadOperation load;
    PC_CORE::StoreOperation store;

    PC_CORE::LoadOperation stencilLoad;
    PC_CORE::StoreOperation stencilStore;

    // TO DO ADD IMAGE STATE 
};

struct SubPassDependcies
{
    GpuPipelineStageFlagBits srcStageMask;
    GpuPipelineStageFlagBits dstStageMask;
  
    GpuAccessFlag srcAccessMask;
    GpuAccessFlag dstAccessMask;
  
};


struct SubPassDescription
{
    ShaderProgramPipelineType shaderProgramPipelineType;
    std::vector<size_t> colorAttachementDescriptorIndicies;
    std::vector<size_t> inputAttachementDescriptorIndicies;

    SubPassDependcies subPassDependcies;
    bool useDepth;
};

struct RenderPassDescriptor
{
    std::vector<RenderPassAttachementDescriptor> attachement;
    RenderPassAttachementDescriptor* depthAttachment;
    
    std::vector<SubPassDescription> subPasses;
};

// this class represent a pass within is frame buffer attemechement
// collection of pass for a same frame buffer
class RhiRenderPass : public RhiResource
{
public:
    
    PC_CORE_API RhiRenderPass(PC_CORE::RHIFormat colorFormat, PC_CORE::RHIFormat depthFormat) {}
    
    PC_CORE_API RhiRenderPass() = default;

    PC_CORE_API RhiRenderPass(const RenderPassDescriptor& _attachementDescriptors) {}
    
    PC_CORE_API virtual ~RhiRenderPass() = default;
    
protected:
    ShaderProgramPipelineType m_ShaderProgramPipelineType;

    uint32_t AttachementCount;
    
    bool m_HasDepth = false;
    
    bool m_HasStencil = false;
};

END_PCCORE
