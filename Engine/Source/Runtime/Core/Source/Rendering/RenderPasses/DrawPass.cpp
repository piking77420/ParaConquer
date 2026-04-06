#include <Rendering/RenderPasses/DrawPass.hpp>

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderSystem.hpp"
#include "Rendering/RenderView.hpp"
#include <LowRenderer/CommandList.hpp>
#include <Rendering/RenderView.hpp>
#include <Resources/StaticMesh.hpp>
#include <World/StaticMeshComponent.hpp>

namespace PC_CORE::Rendering::Pass
{
	DrawPass::DrawPass()
	{
		DYNAMIC_REFLECT_INIT
	}

	DrawPass::~DrawPass() = default;

	size_t DrawPass::PickLodCount(const std::vector<double>& LodThreshold, const MotionCore::Aabb<double>& AABBW, const Tbx::Vector3d& CameraPos, double FovRad) const
	{
		if (LodThreshold.empty())
			return 0;

		double BoundingSphereRadius = (AABBW.GetSize() * 0.5).Magnitude();
		double DistanceToCamera = (AABBW.GetCenter() - CameraPos).Magnitude();
		double ScreenSize = BoundingSphereRadius / (DistanceToCamera * std::tan(FovRad * 0.5));

		for (size_t i = 0; i < LodThreshold.size(); i++)
		{
			if (ScreenSize >= LodThreshold[i])
			{
				return i;
			}
		}
		return LodThreshold.size();
	}

	void DrawPass::DrawStaticMesh(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj, 
		const RhiDescriptorSet* RenderPassSet, size_t FirstSet) const
	{
		if (_DrawObj.UseMeshlet)
		{
			DrawStaticMeshMeshlet(_RendererPassExecuteContext, _DrawObj, RenderPassSet, FirstSet);
		}
		else
		{
			DrawStaticMeshTriangle(_RendererPassExecuteContext, _DrawObj, RenderPassSet, FirstSet);
		}
	}

	void DrawPass::DrawStaticMeshTriangle(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj, const RhiDescriptorSet* RenderPassSet, size_t FirstSet) const
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
		_RendererPassExecuteContext.cmd.BindDescriptorSet(RenderPassSet, FirstSet);

		const auto& StaticMeshData = mesh.GetStaticMeshData();
		const auto& DrawCommands = StaticMeshData.DrawCommands;
		const uint32_t DrawCommandCount = DrawCommands.size() / StaticMeshData.MeshLods.size();

		for (size_t i = 0; i < DrawCommandCount; i++)
		{
			const auto& DrawCommand = DrawCommands[i + (LODIndex * DrawCommandCount)];
			const PC_CORE::MeshSection& Section = StaticMeshData.MeshLods[LODIndex].MeshesSections.at(DrawCommand.MeshSectionIndex);
			const Material* Material = _DrawObj.Materials[Section.MaterialIndex];
			const PC_CORE::Rendering::MaterialType type = _DrawObj.Materials[Section.MaterialIndex]->GetMaterialType();

			if (m_DrawTransperant && type != PC_CORE::Rendering::MaterialType::Opaque)
				continue;

			const RhiDescriptorSet* MaterialDescriptorSet = Material->GetDescriptorSet();

			Gpu::StreamDoubleToFloat(&PushConstant.ModelView, &ModelView);
			Gpu::StreamDoubleToFloat(&PushConstant.NormalInvMatrixView, &NormalInvMatrixView);

			_RendererPassExecuteContext.cmd.BindDescriptorSet(MaterialDescriptorSet, 1, 0);
			_RendererPassExecuteContext.cmd.PushConstant("pushConstant", &PushConstant, sizeof(ModelPushConstant));
			_RendererPassExecuteContext.cmd.DrawIndexed(Section.MeshDataDescriptor.IndicesCount, 1, Section.MeshDataDescriptor.IndicesOffset, Section.MeshDataDescriptor.VertexOffset, 0);
		}

		if (!m_DrawTransperant)
			return;

		_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.transparentForwardShader);
		_RendererPassExecuteContext.cmd.BindDescriptorSet(RenderPassSet, FirstSet);

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

	void DrawPass::DrawStaticMeshMeshlet(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj, const RhiDescriptorSet* RenderPassSet, size_t FirstSet) const
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
}

