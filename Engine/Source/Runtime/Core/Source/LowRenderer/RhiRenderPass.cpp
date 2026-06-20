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
}

