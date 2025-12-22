#pragma once

#include <vector>

#include "RhiTexture.hpp"
#include "RhiBuffer.h"
#include "RhiShaderProgram.hpp"

BEGIN_PCCORE

    struct ImageStateTransition
    {
        RhiTexture* Texture = nullptr;

        RhiResourceState OldState = RhiResourceState::Undefined;
        RhiResourceState NewState = RhiResourceState::Undefined;

        uint32_t FirstMipLevel = 0;
        uint32_t MipLevelsCount = 0;
        uint32_t FirstArraySlice = 0;   
        uint32_t ArraySliceCount = 0;

        bool updateState = false;
    };

    struct BufferStateTransition
    {
        RhiTexture* Texture = nullptr;
        RhiBuffer* Buffer = nullptr;

        RhiResourceState OldState = RhiResourceState::Undefined;
        RhiResourceState NewState = RhiResourceState::Undefined;

        uint32_t Offset = 0;
        uint32_t Size = 0;

        bool updateState = false;
    };

    struct RenderPassAttachementDescriptor
    {
        AttachmentType attachmentType;
        RhiFormat format;
        int sampleCount;

        LoadOperation load;
        StoreOperation store;

        LoadOperation stencilLoad;
        StoreOperation stencilStore;

        RhiResourceState currentImageState;
        RhiResourceState finalImageState;
    };

    struct SubPassTransition
    {
        GpuPipelineStage SrcStageFlag;
        GpuPipelineStage DstStageFlag;
        ImageStateTransition ImageStateTransition;
    };



    struct SubPassDescription
    {
        RhiShaderProgram::PipelineType type;
        std::vector<size_t> colorAttachementDescriptorIndicies;
        std::vector<size_t> inputAttachementDescriptorIndicies;

        SubPassTransition subPassTransition;

        bool useDepth;
    };

    struct RenderPassDescriptor
    {
        std::vector<RenderPassAttachementDescriptor> attachement;
        RenderPassAttachementDescriptor* depthAttachment;

        std::vector<SubPassDescription> subPasses;
    };

    class RhiRenderPass : public RhiObjectT<RhiRenderPass>
    {
    public:
        PC_CORE_API RhiRenderPass() = default;

        PC_CORE_API explicit RhiRenderPass(PC_CORE::Rhi& _Rhi, const std::string& _name, const RenderPassDescriptor& _attachementDescriptors)
            : RhiObjectT(_Rhi, _name)
        {
        }
        
        PC_CORE_API RhiRenderPass(PC_CORE::Rhi& _Rhi, const std::string& _name)
            : RhiObjectT(_Rhi, _name)
        {
        }
        
        PC_CORE_API ~RhiRenderPass() = default;

    protected:        
        RhiShaderProgram::PipelineType m_ShaderProgramPipelineType = RhiShaderProgram::PipelineType::Count;

        uint32_t AttachementCount = 0;

        bool m_HasDepth = false;

        bool m_HasStencil = false;
    };

END_PCCORE
