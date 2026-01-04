#include "Rendering/RenderPasses/ForwardPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/RenderSystem.hpp"


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
			.SetTextureUsage(RhiTexture::TextureUsageFlagBits::LoadAndStore | RhiTexture::TextureUsageFlagBits::RenderTarget | 
				RhiTexture::TextureUsageFlagBits::Sampled)
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


		m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DescriptorSet
			->BindUniformBuffer(RhiShaderStageBits::Vertex, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.SetName("Forward Pass Scene Set")
			.Build();
			

	}

	void FowardPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering)

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

		ViewportInfo viewPort(beginRenderPassInfo.Extent);
		cmd.SetViewPort(viewPort);
		cmd.SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopologyTriangleList);

		cmd.BindProgram(*_RendererPassExecuteContext.Renderer.fowardShader);
		cmd.BindDescriptorSet(*_RendererPassExecuteContext.Renderer.fowardShader, m_DescriptorSet.get(), 0, 1);

		const auto& DrawObjects = _RendererPassExecuteContext.RenderingWorldData.StaticMeshComponentData;
		for (const auto& DrawObject : DrawObjects)
		{
			const StaticMesh& mesh = *DrawObject.StaticMesh;
			const StaticMeshRenderData& Data = mesh.GetStaticMeshRenderData();

			Tbx::Matrix4x4d ModelView = _RendererPassExecuteContext.View.View * DrawObject.WorldMatrix;
			Tbx::Matrix4x4d NormalInvMatrixView = _RendererPassExecuteContext.View.View * DrawObject.NormalInvertMatrix;

			struct ModelPushConstant
			{
				Gpu::mat4 ModelView;
				Gpu::mat4 NormalInvMatrixView;
			}PushConstant;
			Gpu::StreamDoubleToFloat(&PushConstant.ModelView, &ModelView);
			Gpu::StreamDoubleToFloat(&PushConstant.NormalInvMatrixView, &NormalInvMatrixView);
			cmd.PushConstant(*_RendererPassExecuteContext.Renderer.fowardShader, "pushConstant", &PushConstant, sizeof(ModelPushConstant));

			cmd.BindVertexBuffer(*mesh.VBuffer, 0, 1);
			cmd.BindIndexBuffer(*mesh.IBuffer, mesh.IBuffer.GetIndexFormat(), 0);

			for (const auto& SubMesh : Data.SubMeshes)
				cmd.DrawIndexed(SubMesh.IndiciesCount, 1, SubMesh.IndexOffset, SubMesh.VertexOffSet, 0);
			
			
			
		}

		cmd.EndRenderPass();
	}
}