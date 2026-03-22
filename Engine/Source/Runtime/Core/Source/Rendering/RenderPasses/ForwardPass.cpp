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
	}

	void FowardPass::DrawVertex(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj) const
	{
		const StaticMesh& mesh = *_DrawObj.StaticMesh;
		const StaticMeshData& Data = mesh.GetStaticMeshData();

		const Tbx::Matrix4x4d ModelView = _RendererPassExecuteContext.View.View * _DrawObj.WorldMatrix;
		const Tbx::Matrix4x4d NormalInvMatrixView = ModelView.Invert().Transpose();
		MotionCore::Aabb<double> AABBW = _DrawObj.StaticMesh->GetAabb().GetTransformed(_DrawObj.WorldMatrix);
		const uint32_t LODIndex = PickLodCount(mesh.GetLodThreshold(), AABBW, _RendererPassExecuteContext.View.ViewPosition, _RendererPassExecuteContext.View.Fov);

		CommandList::DrawBuffers drawBuffer;
		drawBuffer
			.PushVertexBuffer(
				*mesh.GetVertexBuffer(LODIndex),
				0ull)
			.SetIndexBuffer(
				*mesh.GetIndexBuffer(LODIndex),
				0ull,
				mesh.GetIndexBuffer(LODIndex).GetIndexFormat()
			);
		_RendererPassExecuteContext.cmd.BindDrawBuffers(drawBuffer);

		// material have the the same 
		const size_t MaterialStride = _DrawObj.Materials[0]->GetMaterialStride() * _RendererPassExecuteContext.RHI.GetFrameIndex();

		// TODO SORT SUBMESH SECTION BY METRIAL ID
		struct alignas(16) ModelPushConstant
		{
			Gpu::mat4 ModelView;
			Gpu::mat4 NormalInvMatrixView;
		}PushConstant;

		_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.opaqueFowardShader);
		_RendererPassExecuteContext.cmd.BindDescriptorSet(m_DescriptorSet.get(), 0ull);

		const auto& StaticMeshData = mesh.GetStaticMeshData();
		const auto& DrawCommands = StaticMeshData.DrawCommands;
		const uint32_t DrawCommandCount = DrawCommands.size() / StaticMeshData.MeshLods.size();

		for (size_t i = 0; i < DrawCommandCount; i++)
		{
			const auto& DrawCommand = DrawCommands[i + (LODIndex * DrawCommandCount)];
			const PC_CORE::MeshSection& Section = StaticMeshData.MeshLods[LODIndex].MeshesSections.at(DrawCommand.MeshSectionIndex);
			const Material* Material = _DrawObj.Materials[Section.MaterialIndex];
			const PC_CORE::Rendering::MaterialType type = _DrawObj.Materials[Section.MaterialIndex]->GetMaterialType();

			if (type != PC_CORE::Rendering::MaterialType::Opaque)
				continue;
			const RhiDescriptorSet* MaterialDescriptorSet = Material->GetDescriptorSet();

			Gpu::StreamDoubleToFloat(&PushConstant.ModelView, &ModelView);
			Gpu::StreamDoubleToFloat(&PushConstant.NormalInvMatrixView, &NormalInvMatrixView);

			_RendererPassExecuteContext.cmd.BindDescriptorSet(MaterialDescriptorSet, 1, 0);
			_RendererPassExecuteContext.cmd.PushConstant("pushConstant", &PushConstant, sizeof(ModelPushConstant));
			_RendererPassExecuteContext.cmd.DrawIndexed(Section.MeshDataDescriptor.IndicesCount, 1, Section.MeshDataDescriptor.IndicesOffset, Section.MeshDataDescriptor.VertexOffset, 0);
		}

		_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.transparentForwardShader);
		_RendererPassExecuteContext.cmd.BindDescriptorSet(m_DescriptorSet.get(), 0ull);

		m_TransparentSubMeshDistanceV.clear();
		m_TransparentSubMeshDistanceV.reserve(DrawCommandCount);

		{
			for (size_t i = 0; i < DrawCommandCount; i++)
			{
				const uint32_t DrawCommandIndex = i + (LODIndex * DrawCommandCount);
				const auto& DrawCommand = DrawCommands[DrawCommandIndex];
				const PC_CORE::MeshSection& Section = StaticMeshData.MeshLods[LODIndex].MeshesSections.at(DrawCommand.MeshSectionIndex);
				const PC_CORE::Rendering::MaterialType type = _DrawObj.Materials[Section.MaterialIndex]->GetMaterialType();

				if (type == PC_CORE::Rendering::MaterialType::Transparent)
				{
					MotionCore::Aabb<double> MeshAABB = Section.AABB;
					const Tbx::Vector3d aabbCenterL = (MeshAABB.max - MeshAABB.min);
					const Tbx::Vector4d aabbCenter4V = (ModelView * Tbx::Vector4d(aabbCenterL.x, aabbCenterL.y, aabbCenterL.z, 1.0));
					const Tbx::Vector3d aabbCenterV = Tbx::Vector3d(aabbCenter4V.x, aabbCenter4V.y, aabbCenter4V.z);
					m_TransparentSubMeshDistanceV.emplace_back(std::make_pair(aabbCenterV.MagnitudeSquare(), DrawCommandIndex));
				}
			}
		}

		 // Sort Transparent object based on their view distance
		 // Draw farest item first
		std::ranges::sort(m_TransparentSubMeshDistanceV, [](std::pair<double, uint32_t>& _Left, const std::pair<double, uint32_t>& _Right)
			{
				return _Left.first > _Right.first;
			});

		for (const auto& DrawCommandIndex : m_TransparentSubMeshDistanceV)
		{
			const auto& DrawCommand = DrawCommands[DrawCommandIndex.second];
			const PC_CORE::MeshSection& Section = StaticMeshData.MeshLods[LODIndex].MeshesSections.at(DrawCommand.MeshSectionIndex);
			const Material* Material = _DrawObj.Materials[Section.MaterialIndex];
			const PC_CORE::Rendering::MaterialType type = _DrawObj.Materials[Section.MaterialIndex]->GetMaterialType();

			const RhiDescriptorSet* MaterialDescriptorSet = Material->GetDescriptorSet();

			Gpu::StreamDoubleToFloat(&PushConstant.ModelView, &ModelView);
			Gpu::StreamDoubleToFloat(&PushConstant.NormalInvMatrixView, &NormalInvMatrixView);

			_RendererPassExecuteContext.cmd.BindDescriptorSet(MaterialDescriptorSet, 1, 0);
			_RendererPassExecuteContext.cmd.PushConstant("pushConstant", &PushConstant, sizeof(ModelPushConstant));
			_RendererPassExecuteContext.cmd.DrawIndexed(Section.MeshDataDescriptor.IndicesCount, 1, Section.MeshDataDescriptor.IndicesOffset, Section.MeshDataDescriptor.VertexOffset, 0);
		}
	}

	void FowardPass::DrawMeshLet(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj) const
	{
		const StaticMeshData& Data = _DrawObj.StaticMesh->GetStaticMeshData();
		MotionCore::Aabb<double> AABBW = _DrawObj.StaticMesh->GetAabb().GetTransformed(_DrawObj.WorldMatrix);
		const StaticMesh& mesh = *_DrawObj.StaticMesh;
		const uint32_t LODIndex = PickLodCount(mesh.GetLodThreshold(), AABBW, _RendererPassExecuteContext.View.ViewPosition, _RendererPassExecuteContext.View.Fov);
		const PC_CORE::MeshLOD& LOD = Data.MeshLods[LODIndex];

		struct MeshShaderDrawCall
		{
			Gpu::mat4 ModelViewProjection;
			uint32_t SubMeshMeshletCount;
			uint32_t SubMeshMesletOffset; // 4  68
			uint32_t SubMeshVertexOffset;
			uint32_t SubMeshTriangleVertexOffset;
			uint32_t SubMeshTriangleOffset;
		}
		MeshShaderDrawCall;

		_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.meshShaderMeshlet);

		const Tbx::Matrix4x4d ModelViewProjection = _RendererPassExecuteContext.View.Projection * (_RendererPassExecuteContext.View.View * _DrawObj.WorldMatrix);
		Gpu::StreamDoubleToFloat(&MeshShaderDrawCall.ModelViewProjection, &ModelViewProjection);
		_RendererPassExecuteContext.cmd.BindDescriptorSet(_DrawObj.StaticMesh->GetMeshletDescriptor(LODIndex), 0ull);

		static constexpr auto GroupSize = 32;
		const auto& DrawCommands = Data.DrawCommands;
		const uint32_t DrawCommandCount = DrawCommands.size() / Data.MeshLods.size();
		for (size_t i = 0; i < DrawCommandCount; i++)
		{
			const uint32_t DrawCommandIndex = i + (LODIndex * DrawCommandCount);
			const auto& DrawCommand = DrawCommands[DrawCommandIndex];
			const auto& MeshSection = LOD.MeshesSections[DrawCommand.MeshSectionIndex];
			MeshShaderDrawCall.SubMeshMeshletCount = MeshSection.MeshDataDescriptor.MeshetCount;
			MeshShaderDrawCall.SubMeshMesletOffset = MeshSection.MeshDataDescriptor.MeshetOffset;
			MeshShaderDrawCall.SubMeshVertexOffset = MeshSection.MeshDataDescriptor.VertexOffset;
			MeshShaderDrawCall.SubMeshTriangleVertexOffset = MeshSection.MeshDataDescriptor.MeshletVertexTrianglesIndexOffset;
			MeshShaderDrawCall.SubMeshTriangleOffset = MeshSection.MeshDataDescriptor.MeshletTrianglesOffset;

			_RendererPassExecuteContext.cmd.PushConstant("DrawCall", &MeshShaderDrawCall, sizeof(MeshShaderDrawCall));
			const uint32_t DispachtSize = (MeshSection.MeshDataDescriptor.MeshetCount + GroupSize - 1) / GroupSize;
			_RendererPassExecuteContext.cmd.DrawMeshTask(DispachtSize, 1u, 1u);
		}

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

		// Bind cam and other

		const auto& DrawObjects = _RendererPassExecuteContext.RenderingWorldData.StaticMeshComponentData;
		for (const auto& DrawObject : DrawObjects)
		{
			if (DrawObject.UseMeshlet)
			{
				DrawMeshLet(_RendererPassExecuteContext, DrawObject);
			}
			else
			{
				DrawVertex(_RendererPassExecuteContext, DrawObject);
			}

		}

		cmd.EndRenderPass();
	}

}