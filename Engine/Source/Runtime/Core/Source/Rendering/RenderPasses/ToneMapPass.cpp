#include "Rendering/RenderPasses/ToneMapPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Renderer.hpp"

namespace PC_CORE::Rendering::Pass
{
	ToneMapPass::ToneMapPass()
	{
		DYNAMIC_REFLECT_INIT;
	}

	void ToneMapPass::Build(const RendererPassBuildContext& _RendererPassBuildContext)
	{
		RhiTexture& lightingImage = *_RendererPassBuildContext.RenderGraph.GetResource<RhiTexture>("Lighting Image");
		RhiTexture& outPutImage = _RendererPassBuildContext.RenderGraph.GetOutPutImage();

		m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
		m_FrameBuffer
			->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetAttachement(&outPutImage)
			.SetRenderPass(_RendererPassBuildContext.Renderer.colorLinearPass.get())
			.SetName("ToneMap Framebuffer")
			.Build();

		m_DesciptorSetToneMap.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DesciptorSetToneMap
			->BindImageLoad(RhiShaderStageBits::Compute, 0, &lightingImage)
			.BindImageLoadWrite(RhiShaderStageBits::Compute, 1, &outPutImage)
			.SetName("Tone map descriptorSet")
			.Build();

		m_LightingImageRef = &lightingImage;
		m_OutPutImageRef = &outPutImage;
	}

	void ToneMapPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		ToneMapPassExecute(_RendererPassExecuteContext);
	}

	void ToneMapPass::ToneMapPassExecute(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;
		cmd.BeginComputePasss();

		ImageStateTransition ImageStateTransition
		{
			.Texture = m_LightingImageRef,
			.FirstMipLevel = 0,
			.MipLevelsCount = 1,
			.FirstLayer = 0,
			.LayerCount = 1,

			.updateState = false
		};

		cmd.Barrier(RhiResourceState::PixelShaderResource, RhiResourceState::ComputeRead, std::span(&ImageStateTransition, 1), {});
		ImageStateTransition.Texture = m_OutPutImageRef;
		cmd.Barrier(RhiResourceState::Undefined, RhiResourceState::ComputeReadWrite, std::span(&ImageStateTransition, 1), {});

		const auto LocalSize = _RendererPassExecuteContext.Renderer.toneMapAces->GetLocalSize();
		const auto RenderSize = _RendererPassExecuteContext.View.RenderSize;

		const uint32_t GroupCountX = (RenderSize.x + LocalSize.x - 1) / LocalSize.x;
		const uint32_t GroupCountY = (RenderSize.y + LocalSize.y - 1) / LocalSize.y;

		cmd.BindRhiPipeline(*_RendererPassExecuteContext.Renderer.toneMapAces);
		cmd.BindDescriptorSet(m_DesciptorSetToneMap.get(), 0);
		cmd.Dispatch(GroupCountX, GroupCountY, 1);

		ImageStateTransition.Texture = m_OutPutImageRef;
		cmd.Barrier(RhiResourceState::ComputeReadWrite, RhiResourceState::RenderTarget, std::span(&ImageStateTransition, 1), {});

		cmd.EndRenderPass();
	}
}