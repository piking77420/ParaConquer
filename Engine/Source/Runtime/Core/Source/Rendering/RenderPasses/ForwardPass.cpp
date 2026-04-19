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
			.SetAttachments(m_LightingImage)
			.SetDepthAttachments(&DepthBuffer)
			.SetRenderPass(_RendererPassBuildContext.Renderer.forwardPass.get())
			.SetName("Forward Framebuffer")
			.Build();


		m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DescriptorSet
			->BindUniformBuffer(RhiShaderStageBits::Vertex | RhiShaderStageBits::Pixel, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Pixel, 1, _RendererPassBuildContext.View.LightBuffer.get())
			.BindUniformBuffer(RhiShaderStageBits::Pixel, 2, _RendererPassBuildContext.View.LightBufferHeader.get())
			.SetName("Forward Pass Scene Set")
			.Build();

		m_MeshShaderDescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_MeshShaderDescriptorSet
			->BindUniformBuffer(RhiShaderStageBits::Mesh, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.SetName("Forward Pass Scene Mesh Shader Set")
			.Build();


		m_OnMeshDrawTriangle = [&](const Rendering::DrawStaticMeshTriangle& StaticMesh, PC_CORE::CommandList& _Cmd)
			{
				if (_Cmd.BindProgram(*StaticMesh.ShaderProgram))
				{
					_Cmd.BindDescriptorSet(m_DescriptorSet.get(), 0);
					m_LastMaterialDescriptor = nullptr;
				}

				if (StaticMesh.MaterialDescriptor && m_LastMaterialDescriptor != StaticMesh.MaterialDescriptor)
				{
					m_LastMaterialDescriptor = StaticMesh.MaterialDescriptor;
					const uint32_t MaterialStride = StaticMesh.MaterialDescriptorOffset;
					_Cmd.BindDescriptorSet(m_LastMaterialDescriptor, 1, static_cast<size_t>(MaterialStride));
				}
				CommandList::DrawBuffers drawBuffer;
				drawBuffer
					.PushVertexBuffer(
						*StaticMesh.VertexBuffer,
						0ull)
					.SetIndexBuffer(
						*StaticMesh.IndexBuffer,
						0ull,
						StaticMesh.IndexFormat
					);
				const ModelPushConstant* Push = reinterpret_cast<const ModelPushConstant*>(&StaticMesh.MatrixMV); // hacks

				_Cmd.BindDrawBuffers(drawBuffer);
				_Cmd.PushConstant(RhiShaderStageBits::Vertex, Push, 0u, sizeof(ModelPushConstant));
				_Cmd.DrawIndexed(StaticMesh.IndexCount, 1, StaticMesh.IndexOffset, StaticMesh.VertexOffset, 0);
		};


		m_OnMeshDrawMeshlet = [&](const Rendering::DrawStaticMeshMeshlet& StaticMesh, PC_CORE::CommandList& _Cmd)
			{
				MeshShaderDrawCall MeshShaderDrawCall;
				std::memcpy(MeshShaderDrawCall.ModelView.data.data(), StaticMesh.MatrixMV.data, 16 * sizeof(float));
				MeshShaderDrawCall.SubMeshMeshletCount = StaticMesh.MeshletCount;
				MeshShaderDrawCall.SubMeshMesletOffset = StaticMesh.MeshletOffset;
				MeshShaderDrawCall.SubMeshVertexOffset = StaticMesh.VertexOffset;
				MeshShaderDrawCall.SubMeshTriangleVertexOffset = StaticMesh.SubMeshTriangleVertexOffset;
				MeshShaderDrawCall.SubMeshTriangleOffset = StaticMesh.SubMeshTriangleOffset;

				if (_Cmd.BindProgram(*StaticMesh.ShaderProgram))
				{
					_Cmd.BindDescriptorSet(m_MeshShaderDescriptorSet.get(), 0);
					m_LastMeshletDescritptor = nullptr;
				}
				
				if (StaticMesh.MeshletDescriptor && m_LastMeshletDescritptor != StaticMesh.MeshletDescriptor)
				{
					m_LastMeshletDescritptor = StaticMesh.MeshletDescriptor;
					_Cmd.BindDescriptorSet(StaticMesh.MeshletDescriptor, 1);
				}

				static constexpr auto GroupSize = 32;
				
				_Cmd.PushConstant(RhiShaderStageBits::Amp | RhiShaderStageBits::Mesh, &MeshShaderDrawCall, 0u, sizeof(MeshShaderDrawCall));
				const uint32_t DispachtSize = (StaticMesh.MeshletCount + GroupSize - 1) / GroupSize;
				_Cmd.DrawMeshTask(DispachtSize, 1u, 1u);
		};

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

		ProceedDrawList(_RendererPassExecuteContext.Renderer.OpaqueList, cmd);
		ProceedDrawList(_RendererPassExecuteContext.Renderer.TransparentList, cmd);
		cmd.EndRenderPass();
	}

}