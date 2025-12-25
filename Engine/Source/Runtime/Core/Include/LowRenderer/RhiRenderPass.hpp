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



    struct SubPass
    {
        RhiShaderProgram::PipelineType type;
        std::vector<size_t> colorAttachementDescriptorIndicies;
        std::vector<size_t> inputAttachementIndicies;

        SubPassTransition subPassTransition;

        bool useDepth;
    };

    class RhiRenderPass : public RhiObjectT<RhiRenderPass>
    {
    public:
        PC_CORE_API explicit RhiRenderPass(Rhi& _Rhi);

        PC_CORE_API ~RhiRenderPass() = default;

        // Setter

        RhiRenderPass& SetAttachement(const std::vector<RenderPassAttachementDescriptor>& _RenderPassAttachementDescriptors)
        {
            m_Attachement = _RenderPassAttachementDescriptors;
            return *this;
        }

        RhiRenderPass& SetDepthStencilAttachement(const RenderPassAttachementDescriptor& _RenderPassAttachementDescriptor)
        {
            m_DepthStencilAttachement = _RenderPassAttachementDescriptor;
            return *this;
        }

        RhiRenderPass& SetSubPass(const std::vector<SubPass>& _SubPasses)
        {
            m_SubPasses = _SubPasses;
            return *this;
        }

        // Getter

        const std::vector<RenderPassAttachementDescriptor>& GetAttachement() const
        {
            return m_Attachement;
        }

        const RenderPassAttachementDescriptor& GeDepthStencilAttachement() const
        {
            return m_DepthStencilAttachement;
        }

        const std::vector<SubPass>& GetSubPass() const
        {
            return m_SubPasses;
        }

    protected:        

        std::vector<RenderPassAttachementDescriptor> m_Attachement;

        RenderPassAttachementDescriptor m_DepthStencilAttachement;

        std::vector<SubPass> m_SubPasses;

        bool m_HasDepth = false;

        bool m_HasStencil = false;
    };

END_PCCORE
