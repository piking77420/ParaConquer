#include "Rendering/RenderPasses/ToneMapPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
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

		m_RenderPass.reset(_RendererPassBuildContext.RHI.CreateRenderPass());

		const RenderPassAttachementDescriptor& renderTragetSlot = m_RenderPass
			->CreateAttachment()
			.SetAttachementSlot(AttachementSlot::S00)
			.SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
			.SetSampleCount(1)
			.SetLoadOp(LoadOperation::Clear)
			.SetStoreOp(StoreOperation::Store)
			.SetInitialImageState(RhiResourceState::Undefined)
			.SetFinalImageState(RhiResourceState::FragmentShaderResource);

		m_RenderPass
			->CreateSubPass()
			.SetType(RhiShaderProgram::PipelineType::Graphic)
			.SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));
			
		m_RenderPass
			->SetName("ToneMap temp")
			.Build();

		m_DescriptorSet.reset(_RendererPassBuildContext.Renderer.fowardShader->CreateDescriptorBinding());

		const ImageSamplerDescriptor imageSamplerDescriptor =
		{
			.sampler = _RendererPassBuildContext.Renderer.linearClampToEdgeSampler.get(),
			.texture = &lightingImage,
			.resourceState = RhiResourceState::FragmentShaderResource
		};

		const DescriptorWrite Write = {
			.type = DescriptorType::CombinedImageSampler,
			.bindingIndex = 0,
			.descriptor = imageSamplerDescriptor
		};


		m_DescriptorSet
			->SetBindings(0, Write)
			.SetName("Desciptor Set Binding Lighting Image to final Image")
			.Build();
	}

	void ToneMapPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		CommandList& cmd = _RendererPassExecuteContext.cmd;

		std::array<float, 4> Color = GetColor();

		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = m_RenderPass.get(),
			.FrameBuffer = m_FrameBuffer.get(),
			.RenderOffSet = {0, 0},
			.Extent = {m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight()},
			.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
			.ClearColor = &Color,
			.ClearValueCount = 1,
		};
		cmd.BeginRenderPass(beginRenderPassInfo);

		cmd.BindProgram(*_RendererPassExecuteContext.Renderer.drawTextureQuad);
		cmd.BindDescriptorSet(*_RendererPassExecuteContext.Renderer.drawTextureQuad, m_DescriptorSet.get(), 0, 1);
		cmd.Draw(3, 1, 0, 0);

		cmd.EndRenderPass();
	}
}