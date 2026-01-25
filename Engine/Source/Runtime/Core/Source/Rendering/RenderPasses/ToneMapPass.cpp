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
		const RhiTexture& outPutImage = _RendererPassBuildContext.RenderGraph.GetOutPutImage();

	 
		m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
		m_FrameBuffer
			->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetAttachments(&outPutImage)
			.SetRenderPass(_RendererPassBuildContext.Renderer.drawTextureQuadPass.get())
			.SetName("ToneMap Framebuffer")
			.Build();

		m_DesciptorSetDrawQuad.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		RhiSampler* sampler = _RendererPassBuildContext.Renderer.linearClampToEdgeSampler.get();
		m_DesciptorSetDrawQuad
			->BindTexture(RhiShaderStageBits::Pixel, 0, &lightingImage, sampler)
			.SetName("Desciptor Set Binding Lighting Image to final Image")
			.Build();


		m_DesciptorSetToneMap.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DesciptorSetToneMap
			->BindImageLoadWrite(RhiShaderStageBits::Compute, 0, &lightingImage)
			.SetName("Tone map descriptorSet")
			.Build();

		m_LightingImageRef = &lightingImage;
	}

	void ToneMapPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		ToneMapPassExecute(_RendererPassExecuteContext);
		DrawToViewPort(_RendererPassExecuteContext);
	}

	void ToneMapPass::ToneMapPassExecute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		const ImageStateTransition ImageStateTransition
		{
			.Texture = m_LightingImageRef,
			.FirstMipLevel = 0,
			.MipLevelsCount = 1,
			.FirstLayer = 0,
			.LayerCount = 1,

			.updateState = false
		};

		cmd.Barrier(RhiResourceState::FragmentShaderResource, RhiResourceState::ComputeWrite, std::span(&ImageStateTransition, 1), {});

		const auto LocalSize = _RendererPassExecuteContext.Renderer.toneMapAces->GetLocalSize();
		const auto RenderSize = _RendererPassExecuteContext.View.RenderSize;

		const uint32_t GroupCountX =
			(RenderSize.x + LocalSize.x - 1) / LocalSize.x;

		const uint32_t GroupCountY =
			(RenderSize.y + LocalSize.y - 1) / LocalSize.y;

		cmd.BindProgram(*_RendererPassExecuteContext.Renderer.toneMapAces);
		cmd.BindDescriptorSet(m_DesciptorSetToneMap.get(), 0);
		cmd.Dispatch(GroupCountX, GroupCountY, 1);

	}


	void ToneMapPass::DrawToViewPort(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		const ImageStateTransition LightingImageTransition
		{
			.Texture = m_LightingImageRef,
			.FirstMipLevel = 0,
			.MipLevelsCount = 1,
			.FirstLayer = 0,
			.LayerCount = 1,

			.updateState = false
		};

		cmd.Barrier(RhiResourceState::ComputeReadWrite, RhiResourceState::FragmentShaderResource, std::span(&LightingImageTransition, 1), {});

		std::array<float, 4> Color = GetColor();

		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = _RendererPassExecuteContext.Renderer.drawTextureQuadPass.get(),
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
		cmd.BindDescriptorSet(m_DesciptorSetDrawQuad.get(), 0);
		cmd.Draw(3, 1, 0, 0);

		cmd.EndRenderPass();

		const ImageStateTransition OutPutImageStateTransition
		{
			.Texture = &_RendererPassExecuteContext.RenderGraph.GetOutPutImage(),
			.FirstMipLevel = 0,
			.MipLevelsCount = 1,
			.FirstLayer = 0,
			.LayerCount = 1,

			.updateState = false
		};

		cmd.Barrier(RhiResourceState::Undefined, RhiResourceState::FragmentShaderResource, std::span(&OutPutImageStateTransition, 1), {});
	}
}