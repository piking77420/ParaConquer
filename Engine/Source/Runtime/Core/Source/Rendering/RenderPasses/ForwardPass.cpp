#include "Rendering/RenderPasses/ForwardPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderView.hpp"

namespace PC_CORE::Rendering::Pass
{

	FowardPass::FowardPass()
	{
		DYNAMIC_REFLECT_INIT;
	}

	void FowardPass::Build(const RendererPassBuildContext& _RendererPassBuildContext)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering);

		RhiTexture& lightingImage = _RendererPassBuildContext.RenderGraph.CreateResourceHandle<RhiTexture>("Lighting Image");
		lightingImage
			.SetMemoryUsage(RhiMemoryUsage::Dynamic)
			.SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
			.SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetTextureUsage(RhiTexture::TextureUsageFlagBits::LoadAndStore | RhiTexture::TextureUsageFlagBits::RenderTarget)
			.SetSamples(1) // TODO MUTIPLE MSAA SETTING FROM VIEW
			.Build();

		RhiTexture& DepthBuffer = _RendererPassBuildContext.RenderGraph.CreateResourceHandle<RhiTexture>("Depth Buffer");
		DepthBuffer
			.SetTextureType(RhiTexture::Type::Texture2D)
			.SetMemoryUsage(RhiMemoryUsage::Dynamic)
			.SetRhiFormat(RhiFormat::D24UnormS8Uint)
			.SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetTextureUsage(RhiTexture::TextureUsageFlagBits::DepthStencil)
			.SetSamples(1)
			.SetName("Depth Image")
			.Build();


		m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
		m_FrameBuffer
			->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetAttachments(&lightingImage)
			.SetDepthAttachments(&DepthBuffer)
			.SetRenderPass(_RendererPassBuildContext.Renderer.forwardPass.get())
			.SetName("Forward Framebuffer")
			.Build();
	}

	void FowardPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		std::array<float, 4> Color = GetColor();
		
		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = _RendererPassExecuteContext.Renderer.forwardPass.get(),
			.FrameBuffer = m_FrameBuffer.get(),
			.RenderOffSet = {0, 0},
			.Extent = {m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight()},
			.ClearValueFlag = ClearValueFlagBits::ClearValueColor | ClearValueFlagBits::ClearValueDepth,
			.ClearColor = &Color,
			.ClearValueCount = 1,
			.ClearDepth = 1.f
		};
		cmd.BeginRenderPass(beginRenderPassInfo);




		cmd.EndRenderPass();
	}
}