#include "Rendering/RenderPasses/ForwardPass.hpp"

#include "LowRenderer/Rhi.hpp"
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


		m_RenderPass.reset(_RendererPassBuildContext.RHI.CreateRenderPass());

		const RenderPassAttachementDescriptor& ColorAttachement = m_RenderPass
			->CreateAttachment()
			.SetAttachementSlot(AttachementSlot::S00)
			.SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
			.SetSampleCount(1)
			.SetLoadOp(LoadOperation::Clear)
			.SetStoreOp(StoreOperation::Store)
			.SetInitialImageState(RhiResourceState::Undefined)
			.SetFinalImageState(RhiResourceState::FragmentShaderResource);

		// Set Depth
		const RenderPassAttachementDescriptor& DepthAttachement = m_RenderPass
			->CreateAttachment()
			.SetAttachementSlot(AttachementSlot::S01)
			.SetRhiFormat(RhiFormat::D24UnormS8Uint)
			.SetSampleCount(1)
			.SetLoadOp(LoadOperation::Clear)
			.SetStoreOp(StoreOperation::Store)
			.SetInitialImageState(RhiResourceState::DepthStencilWrite)
			.SetFinalImageState(RhiResourceState::DepthStencilWrite);

		// SubPass 0
		m_RenderPass
			->CreateSubPass()
			.SetType(RhiShaderProgram::PipelineType::Graphic)
			.SetAttachementRef(AttachementRef(ColorAttachement, RhiResourceState::RenderTarget))
			.SetDepthAttachementRef(AttachementRef(DepthAttachement, RhiResourceState::DepthStencilWrite));

		m_RenderPass
			->SetName("ForwardPass")
			.Build();
	}

	void FowardPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		std::array<Tbx::Vector4f, 1> clearValues = {
			Tbx::Vector4f(0.1, 0.1, 0.1, 1.f),
		};
		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = m_RenderPass.get(),
			.FrameBuffer = m_CurrentView->FrameBuffers.ForwardFrameBuffer,
			.RenderOffSet = {0, 0},
			.Extent = {rContextView.RenderingContextSize.x, rContextView.RenderingContextSize.y},
			.ClearValueFlag = ClearValueFlagBits::ClearValueColor | ClearValueFlagBits::ClearValueDepth,
			.ClearColor = clearValues.data(),
			.ClearValueCount = clearValues.size(),
			.ClearDepth = 1.f
		};
		cmd.BeginRenderPass()
	}
}