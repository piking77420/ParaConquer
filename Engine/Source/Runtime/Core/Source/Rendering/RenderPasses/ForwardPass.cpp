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
			->BindUniformBuffer(RhiShaderStageBits::Vertex, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Pixel, 1, _RendererPassBuildContext.View.LightBuffer.get())
			.BindUniformBuffer(RhiShaderStageBits::Pixel, 2, _RendererPassBuildContext.View.LightBufferHeader.get())
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
		const auto& DrawObjects = _RendererPassExecuteContext.RenderingWorldData.StaticMeshComponentData;
		for (const auto& DrawObject : DrawObjects)
		{
			const StaticMesh& mesh = *DrawObject.StaticMesh;
			const StaticMeshRenderData& Data = mesh.GetStaticMeshRenderData();

			const Tbx::Matrix4x4d ModelView = _RendererPassExecuteContext.View.View * DrawObject.WorldMatrix;
			const Tbx::Matrix4x4d NormalInvMatrixView = ModelView.Invert().Transpose();

			CommandList::DrawBuffers drawBuffer;
			drawBuffer
				.PushVertexBuffer(
					*mesh.GetVertexBuffer()
					, 0ull)
				.SetIndexBuffer(
					*mesh.GetIndexBuffer()
					, 0ull
					, mesh.GetIndexBuffer().GetIndexFormat()
				);
			cmd.BindDrawBuffers(drawBuffer);

			// material have the the same 
			const size_t MaterialStride = DrawObject.Materials[0]->GetMaterialStride() * _RendererPassExecuteContext.RHI.GetFrameIndex();

			// TODO SORT SUBMESH SECTION BY METRIAL ID
			struct alignas(16) ModelPushConstant
			{
				Gpu::mat4 ModelView;
				Gpu::mat4 NormalInvMatrixView;
			}PushConstant;

			cmd.BindProgram(*_RendererPassExecuteContext.Renderer.opaqueFowardShader);
			for (const auto& SubMesh : Data.SubMeshes)
			{
				PC_CORE::Rendering::MaterialType type = DrawObject.Materials[SubMesh.MaterialIndex]->GetMaterialType();

				if (type != PC_CORE::Rendering::MaterialType::Opaque)
					continue;
				// TODO MOVE THIS per mesh or subsidvce submesh -> static mehs
				
				Gpu::StreamDoubleToFloat(&PushConstant.ModelView, &ModelView);
				Gpu::StreamDoubleToFloat(&PushConstant.NormalInvMatrixView, &NormalInvMatrixView);
				cmd.PushConstant("pushConstant", &PushConstant, sizeof(ModelPushConstant));


				cmd.BindDescriptorSet(DrawObject.Materials[SubMesh.MaterialIndex]->GetDescriptorSet(), 1, MaterialStride);
				cmd.BindDescriptorSet(m_DescriptorSet.get(), 0ull);

				cmd.DrawIndexed(SubMesh.IndiciesCount, 1, SubMesh.IndexOffset, SubMesh.VertexOffSet, 0);
			}

			cmd.BindProgram(*_RendererPassExecuteContext.Renderer.transparentForwardShader);
			m_TransparentSubMeshDistanceV.clear();
			m_TransparentSubMeshDistanceV.reserve(Data.SubMeshes.size());

			{
				uint32_t Index = 0;
				for (const auto& SubMesh : Data.SubMeshes)
				{
					const PC_CORE::Rendering::MaterialType type = DrawObject.Materials[SubMesh.MaterialIndex]->GetMaterialType();

					if (type == PC_CORE::Rendering::MaterialType::Transparent)
					{
						const Tbx::Vector3d aabbCenterL = (SubMesh.AABB.max - SubMesh.AABB.min);
						const Tbx::Vector4d aabbCenter4V = (ModelView * Tbx::Vector4d(aabbCenterL.x, aabbCenterL.y, aabbCenterL.z, 1.0));
						const Tbx::Vector3d aabbCenterV = Tbx::Vector3d(aabbCenter4V.x, aabbCenter4V.y, aabbCenter4V.z);
						m_TransparentSubMeshDistanceV.emplace_back(std::make_pair(aabbCenterV.MagnitudeSquare(), Index));
					}
					Index++;
				}
			}
			
			// Sort Transparent object based on their view distance
			// Draw farest item first
			std::ranges::sort(m_TransparentSubMeshDistanceV, [](std::pair<double, uint32_t>& _Left, const std::pair<double, uint32_t>& _Right) 
				{
					return _Left.first > _Right.first;
				});

			for (const auto& item : m_TransparentSubMeshDistanceV)
			{
				const auto& SubMesh = Data.SubMeshes[item.second];

				Gpu::StreamDoubleToFloat(&PushConstant.ModelView, &ModelView);
				Gpu::StreamDoubleToFloat(&PushConstant.NormalInvMatrixView, &NormalInvMatrixView);
				cmd.PushConstant("pushConstant", &PushConstant, sizeof(ModelPushConstant));
				cmd.BindDescriptorSet(DrawObject.Materials[SubMesh.MaterialIndex]->GetDescriptorSet(), 1, MaterialStride);
				cmd.BindDescriptorSet(m_DescriptorSet.get(), 0ull);

				cmd.DrawIndexed(SubMesh.IndiciesCount, 1, SubMesh.IndexOffset, SubMesh.VertexOffSet, 0);
			}

		}



		cmd.BindProgram(*_RendererPassExecuteContext.Renderer.meshShader);
		cmd.DrawMeshTask(1, 1, 1);

		cmd.EndRenderPass();
	}

}