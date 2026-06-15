#include "Rendering/RenderPasses/ForwardPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/RenderSystem.hpp"
#include "App.hpp"

namespace PC_CORE::Rendering::Pass
{

	FowardPass::FowardPass()
	{
		DYNAMIC_REFLECT_INIT;

		m_DrawTransperant = true;
	}

	void FowardPass::Build(const RendererPassBuildContext& _RendererPassBuildContext)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering);

		m_LightingImage = &_RendererPassBuildContext.RenderGraph.CreateResourceHandle<RhiTexture>("Lighting Image");
		m_LightingImage
			->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
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
			.SetMemoryUsage(RhiMemoryUsage::StaticGPU)
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
			.SetAttachement(m_LightingImage)
			.SetDepthAttachment(&DepthBuffer)
			.SetRenderPass(_RendererPassBuildContext.Renderer.forwardPass.get())
			.SetName("Forward Framebuffer")
			.Build();


		m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DescriptorSet
			->BindUniformBuffer(RhiShaderStageBits::Vertex | RhiShaderStageBits::Pixel, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Vertex, 1, _RendererPassBuildContext.Renderer.InstanceBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Pixel, 2, _RendererPassBuildContext.View.LightBuffer.get())
			.BindUniformBuffer(RhiShaderStageBits::Pixel, 3, _RendererPassBuildContext.View.LightBufferHeader.get())
			.SetName("Forward Pass Scene Set")
			.Build();

		m_DescriptorMeshlet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DescriptorMeshlet
			->BindUniformBuffer(RhiShaderStageBits::Mesh | RhiShaderStageBits::Amp | RhiShaderStageBits::Pixel, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Mesh | RhiShaderStageBits::Amp, 1, _RendererPassBuildContext.Renderer.InstanceBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Pixel, 2, _RendererPassBuildContext.View.LightBuffer.get())
			.BindUniformBuffer(RhiShaderStageBits::Pixel, 3, _RendererPassBuildContext.View.LightBufferHeader.get())
			.SetName("Forward Pass Scene Set")
			.Build();


		m_OnMeshDrawTriangle = [&](const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const Rendering::DrawStaticMeshTriangle& StaticMesh)
		{
			if (_Context.cmd.BindProgram(*StaticMesh.ShaderProgram))
			{
				_Context.cmd.BindDescriptorSet(m_DescriptorSet.get(), 0);
				if (_Context.RenderingWorldData.EnvironemementDescriptorSet)
					_Context.cmd.BindDescriptorSet(*_Context.RenderingWorldData.EnvironemementDescriptorSet, 2);
				m_LastMaterialDescriptor = nullptr;
			}

			if (StaticMesh.MaterialDescriptor && m_LastMaterialDescriptor != StaticMesh.MaterialDescriptor)
			{
				m_LastMaterialDescriptor = StaticMesh.MaterialDescriptor;
				const uint32_t MaterialStride = StaticMesh.MaterialDescriptorOffset;
				_Context.cmd.BindDescriptorSet(m_LastMaterialDescriptor, 1, static_cast<size_t>(MaterialStride));
			}
		};

		m_OnMeshDrawMeshlet = [&](const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const Rendering::DrawStaticMeshMeshlet& StaticMesh)
		{
			if (_Context.cmd.BindProgram(*StaticMesh.ShaderProgram))
			{
				_Context.cmd.BindDescriptorSet(m_DescriptorMeshlet.get(), 0);
				if (_Context.RenderingWorldData.EnvironemementDescriptorSet)
					_Context.cmd.BindDescriptorSet(*_Context.RenderingWorldData.EnvironemementDescriptorSet, 2);
			}

			if (StaticMesh.MaterialDescriptor && m_LastMaterialDescriptor != StaticMesh.MaterialDescriptor)
			{
				m_LastMaterialDescriptor = StaticMesh.MaterialDescriptor;
				const uint32_t MaterialStride = StaticMesh.MaterialDescriptorOffset;
				_Context.cmd.BindDescriptorSet(m_LastMaterialDescriptor, 1, static_cast<size_t>(MaterialStride));
			}
		};

	}

	void FowardPass::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering)

		CommandList& cmd = _RendererPassExecuteContext.cmd;

		std::array<float, 4> Color = { 0.0f, 0.0f, 0.0f, 0.0f };
		
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

		ProceedDrawList(_RendererPassExecuteContext, _RendererPassExecuteContext.Renderer.OpaqueList);
		ProceedDrawList(_RendererPassExecuteContext, _RendererPassExecuteContext.Renderer.TransparentList);
		ProceedDrawList(_RendererPassExecuteContext, _RendererPassExecuteContext.Renderer.Skybox);
		cmd.EndRenderPass();
	}

}