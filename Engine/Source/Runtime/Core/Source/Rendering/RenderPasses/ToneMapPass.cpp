#include "Rendering/RenderPasses/ToneMapPass.hpp"

namespace PC_CORE::Rendering::Pass
{
	ToneMapPass::ToneMapPass()
	{
		DYNAMIC_REFLECT_INIT;
	}

	void ToneMapPass::Build(const RendererPassBuildContext& _RendererPassBuildContext)
	{
	}

	void ToneMapPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
	}
}