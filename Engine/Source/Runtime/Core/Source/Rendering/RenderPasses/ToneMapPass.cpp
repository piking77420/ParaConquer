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
		const RhiTexture& lightingImage = _RendererPassBuildContext.RenderGraph.GetResource<RhiTexture>("Lighting Image");
		const RhiTexture& outPutImage = _RendererPassBuildContext.RenderGraph.GetOutPutImage();

	 
		m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
		m_FrameBuffer
			->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetAttachments(&outPutImage)
			.SetRenderPass(_RendererPassBuildContext.Renderer.toneMapPass.get())
			.SetName("ToneMap Framebuffer")
			.Build();

		m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		RhiSampler* sampler = _RendererPassBuildContext.Renderer.linearClampToEdgeSampler.get();
		m_DescriptorSet
			->BindTexture(RhiShaderStageBits::Pixel, 0, &lightingImage, sampler)
			.SetName("Desciptor Set Binding Lighting Image to final Image")
			.Build();
	}

	void ToneMapPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		std::array<float, 4> Color = GetColor();

		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = _RendererPassExecuteContext.Renderer.toneMapPass.get(),
			.FrameBuffer = m_FrameBuffer.get(),
			.RenderOffSet = {0, 0},
			.Extent = {m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight()},
			.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
			.ClearColor = &Color,
			.ClearValueCount = 1,
		};
		cmd.BeginRenderPass(beginRenderPassInfo);

		ViewportInfo viewPort(beginRenderPassInfo.Extent);
		cmd.SetViewPort(viewPort);
		cmd.SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopologyTriangleList);

		cmd.BindProgram(*_RendererPassExecuteContext.Renderer.drawTextureQuad);
		cmd.BindDescriptorSet(*_RendererPassExecuteContext.Renderer.drawTextureQuad, m_DescriptorSet.get(), 0, 1);
		cmd.Draw(3, 1, 0, 0);

		cmd.EndRenderPass();

		const ImageStateTransition ImageStateTransition
		{
			.Texture = &_RendererPassExecuteContext.RenderGraph.GetOutPutImage(),
			.FirstMipLevel = 0,
			.MipLevelsCount = 1,
			.FirstLayer = 0,
			.LayerCount = 1,

			.updateState = false
		};

		cmd.Barrier(RhiResourceState::FragmentShaderResource, RhiResourceState::FragmentShaderResource, std::span(&ImageStateTransition, 1), {});
	}
}