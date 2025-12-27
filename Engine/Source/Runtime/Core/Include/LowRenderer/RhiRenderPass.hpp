#pragma once

#include <vector>

#include "RhiTexture.hpp"
#include "RhiBuffer.h"
#include "RhiShaderProgram.hpp"

BEGIN_PCCORE

enum struct AttachementSlot : uint8_t
{
    S00,
    S01,
    S02,
    S03,
    S04,
    S05,
    S06,
    S07,
    S08,
    S09,
    S10,
    S11,
    S12,
    S13,
    S14,
    S15,
    S16,
    S17,
    S18,
    S19,
    Count,
};
static_assert(static_cast<uint32_t>(AttachementSlot::Count) == 20u);



struct RenderPassAttachementDescriptor
{
    AttachementSlot Slot{ AttachementSlot::Count };
    RhiFormat Format{ RhiFormat::Undefined };
    uint8_t SampleCount{1}; // TO DO CHANGE IT FOR ENUM

    LoadOperation Load{ LoadOperation::DontCare };
    StoreOperation Store{ StoreOperation::DontCare };

    LoadOperation StencilLoad{ LoadOperation::DontCare };
    StoreOperation StencilStore{ StoreOperation::DontCare };

    RhiResourceState InitialImageState{ RhiResourceState::Undefined };
    RhiResourceState FinalImageState{ RhiResourceState::Undefined };

    RenderPassAttachementDescriptor& SetAttachementSlot(AttachementSlot AttachementSlot)
    {
        Slot = AttachementSlot;
        return *this;
    }

    RenderPassAttachementDescriptor& SetRhiFormat(RhiFormat _Format)
    {
        Format = _Format;
        return *this;
    }

    RenderPassAttachementDescriptor& SetSampleCount(int _SampleCount)
    {
        SampleCount = _SampleCount;
        return *this;
    }

    RenderPassAttachementDescriptor& SetLoadOp(LoadOperation LoadOperation)
    {
        Load = LoadOperation;
        return *this;
    }

    RenderPassAttachementDescriptor& SetStoreOp(StoreOperation StoreOperation)
    {
        Store = StoreOperation;
        return *this;
    }

    RenderPassAttachementDescriptor& SetStencilLoadOp(LoadOperation LoadOperation)
    {
        Load = LoadOperation;
        return *this;
    }

    RenderPassAttachementDescriptor& SetStencilStoreOp(StoreOperation StoreOperation)
    {
        Store = StoreOperation;
        return *this;
    }

    RenderPassAttachementDescriptor& SetInitialImageState(RhiResourceState _RhiResourceState)
    {
        InitialImageState = _RhiResourceState;
        return *this;
    }

    RenderPassAttachementDescriptor& SetFinalImageState(RhiResourceState _RhiResourceState)
    {
        FinalImageState = _RhiResourceState;
        return *this;
    }
};

struct AttachementRef
{
    AttachementSlot Slot = AttachementSlot::Count;
    RhiResourceState ResourceState{RhiResourceState::Undefined};

    AttachementRef() = default;

    AttachementRef(const RenderPassAttachementDescriptor& _RenderPassAttachementDescriptor,
        RhiResourceState _RhiResourceState)
    {
        Slot = _RenderPassAttachementDescriptor.Slot;
        ResourceState = _RhiResourceState;
    }

    ~AttachementRef() = default;
};

template<typename T>
concept AttachementT =
std::same_as<std::remove_cvref_t<T>, AttachementRef>;

struct SubPass
{
    RhiShaderProgram::PipelineType type{};

    std::vector<AttachementRef> ColorAttachements{};

    std::vector<AttachementRef> InputAttachements{};

    AttachementRef DepthAttachement{};

    GpuPipelineStage SrcStageFlag;

    GpuPipelineStage DstStageFlag;

    SubPass& SetType(RhiShaderProgram::PipelineType _Type)
    {
        type = _Type;
        return *this;
    }

    template <AttachementT... Refs>
    SubPass& SetAttachementRef(Refs&&... _Args)
    {
        (ColorAttachements.emplace_back(std::forward<Refs>(_Args)), ...);
        return *this;
    }

    template <AttachementT... Refs>
    SubPass& SetInputAttachementRef(Refs&&... _Args)
    {
        (InputAttachements.emplace_back(std::forward<Refs>(_Args)), ...);
        return *this;
    }

    SubPass& SetDepthAttachementRef(AttachementRef _AttachementRef)
    {
        DepthAttachement = std::move(_AttachementRef);
        return *this;
    }

    SubPass& SetSrcStageFlag(GpuPipelineStage _SrcStageFlag)
    {
        SrcStageFlag = _SrcStageFlag;
        return *this;
    }

    SubPass& SetDstStageFlag(GpuPipelineStage _DstStageFlag)
    {
        DstStageFlag = _DstStageFlag;
        return *this;
    }

    bool HasDepthAttachement() const
    {
        return DepthAttachement.Slot != AttachementSlot::Count;
    }

};

class RhiRenderPass : public RhiObjectT<RhiRenderPass>
{
public:
    static constexpr size_t MaxAttachementSlot = static_cast<uint32_t>(AttachementSlot::Count);
    static constexpr size_t MaxSubPass = 10u;


    PC_CORE_API explicit RhiRenderPass(Rhi& _Rhi);

    PC_CORE_API ~RhiRenderPass() override = default;
    
    RhiRenderPass& ClearContext()
    {
        m_SubPassCount = 0;
        m_AttachementCount = 0;

        m_SubPasses = {};
        m_Attachements = {};

        return *this;
    }

    RenderPassAttachementDescriptor& CreateAttachment()
    {
        assert((m_AttachementCount + 1) < MaxAttachementSlot);

        return m_Attachements[m_AttachementCount++];
    }

    SubPass& CreateSubPass()
    {
        assert((m_SubPassCount + 1) < MaxSubPass);

        return m_SubPasses[m_SubPassCount++];
    }

       

protected:
    size_t m_AttachementCount{ 0 };

    size_t m_SubPassCount{ 0 };

    std::array<RenderPassAttachementDescriptor, MaxAttachementSlot> m_Attachements;

    std::array<SubPass, MaxSubPass> m_SubPasses;
};

END_PCCORE
