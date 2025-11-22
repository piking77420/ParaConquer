#pragma once

#include <Vector>

#include "RhiTypedef.h"
#include "RhiResource.hpp"

BEGIN_PCCORE
    struct RenderPassAttachementDescriptor
    {
        AttachmentType attachmentType;
        RhiFormat format;
        int sampleCount;

        LoadOperation load;
        StoreOperation store;

        LoadOperation stencilLoad;
        StoreOperation stencilStore;

        ImageState currentImageState;
        ImageState finalImageState;
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
        PC_CORE_API RhiRenderPass(RhiFormat colorFormat, RhiFormat depthFormat)
        {
        }

        PC_CORE_API RhiRenderPass() = default;

        PC_CORE_API RhiRenderPass(const RenderPassDescriptor& _attachementDescriptors)
        {
        }

        PC_CORE_API ~RhiRenderPass() override = default;

    protected:
        ShaderProgramPipelineType m_ShaderProgramPipelineType;

        uint32_t AttachementCount;

        bool m_HasDepth = false;

        bool m_HasStencil = false;
    };

END_PCCORE
