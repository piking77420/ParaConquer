#include "LowRenderer/RhiRenderPass.hpp"

namespace PC_CORE
{
	RhiRenderPass::RhiRenderPass(Rhi& _Rhi)
		: RhiObjectT(_Rhi)
	{

	}

	const std::array<SubPass, RhiRenderPass::MaxSubPass>& RhiRenderPass::GetSubPasses() const
	{
		return m_SubPasses;
	}

	const std::array<RenderPassAttachementDescriptor, RhiRenderPass::MaxAttachementSlot>& RhiRenderPass::GetAttachementDescriptor() const
	{
		return m_Attachements;
	}

	uint32_t RhiRenderPass::Hash() const
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rhi);

		uint32_t Seed = 0u;

		HashCombine(Seed, m_AttachementCount);
		HashCombine(Seed, m_SubPassCount);

		for (size_t i = 0; i < m_AttachementCount; i++)
		{
			const RenderPassAttachementDescriptor& Att = m_Attachements[i];
			HashCombine(Seed, static_cast<uint32_t>(Att.Slot));
			HashCombine(Seed, static_cast<uint32_t>(Att.Format));
			HashCombine(Seed, static_cast<uint32_t>(Att.SampleCount));
			HashCombine(Seed, static_cast<uint32_t>(Att.Load));
			HashCombine(Seed, static_cast<uint32_t>(Att.Store));
			HashCombine(Seed, static_cast<uint32_t>(Att.StencilLoad));
			HashCombine(Seed, static_cast<uint32_t>(Att.StencilStore));
			HashCombine(Seed, static_cast<uint32_t>(Att.InitialImageState));
			HashCombine(Seed, static_cast<uint32_t>(Att.FinalImageState));
		}

		for (size_t i = 0; i < m_SubPassCount; i++)
		{
			const SubPass& SubPass = m_SubPasses[i];

			HashCombine(Seed, static_cast<uint32_t>(SubPass.ColorAttachements.size()));
			for (const auto& AttRefColor : SubPass.ColorAttachements)
			{
				HashCombine(Seed, static_cast<uint32_t>(AttRefColor.Slot));
				HashCombine(Seed, static_cast<uint32_t>(AttRefColor.ResourceState));
			}

			std::vector<AttachementRef> InputAttachements{};
			for (const auto& InputAttachement : SubPass.ColorAttachements)
			{
				HashCombine(Seed, static_cast<uint32_t>(InputAttachement.Slot));
				HashCombine(Seed, static_cast<uint32_t>(InputAttachement.ResourceState));
			}
				
			const bool HasAttachement = SubPass.HasDepthAttachement();
			HashCombine(Seed, static_cast<uint32_t>(HasAttachement));
			if (HasAttachement)
			{
				HashCombine(Seed, static_cast<uint32_t>(SubPass.DepthAttachement.Slot));
				HashCombine(Seed, static_cast<uint32_t>(SubPass.DepthAttachement.ResourceState));
			}
		}

		return Seed;
	}
}

