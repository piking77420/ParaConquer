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

	DrawPass::DrawPass(OnMeshDrawTriangleSignature _OnMeshDrawTriangleSignature, OnMeshDrawMeshletSignature _OnMeshDrawMeshlet)
		: m_OnMeshDrawTriangle(_OnMeshDrawTriangleSignature)
		, m_OnMeshDrawMeshlet(_OnMeshDrawMeshlet)
	{

	}

	DrawPass::~DrawPass() = default;

	void DrawPass::ProceedDrawList(const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const DrawList& _DrawList) const
	{
		m_LastMeshletDescritptor = nullptr;
		m_LastMaterialDescriptor = nullptr;

		for (const auto& DrawItem : _DrawList)
		{
			std::visit(overloaded{
				  [&](const Rendering::DrawStaticMeshTriangle& StaticMesh) {
					if (m_OnMeshDrawTriangle)
					{
						m_OnMeshDrawTriangle(_Context, StaticMesh);

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

						_Context.cmd.BindDrawBuffers(drawBuffer);
						_Context.cmd.PushConstant(RhiShaderStageBits::Vertex, Push, 0u, sizeof(ModelPushConstant));
						_Context.cmd.DrawIndexed(StaticMesh.IndexCount, 1, StaticMesh.IndexOffset, StaticMesh.VertexOffset, 0);
					}

				},
				[&](const Rendering::DrawStaticMeshMeshlet& StaticMesh) {
					if (m_OnMeshDrawMeshlet)
					{
						m_OnMeshDrawMeshlet(_Context, StaticMesh);

						if (StaticMesh.MeshletDescriptor && m_LastMeshletDescritptor != StaticMesh.MeshletDescriptor)
						{
							m_LastMeshletDescritptor = StaticMesh.MeshletDescriptor;
							_Context.cmd.BindDescriptorSet(StaticMesh.MeshletDescriptor, 2);
						}
						MeshShaderDrawCall MeshShaderDrawCall;
						std::memcpy(MeshShaderDrawCall.ModelView.data.data(), StaticMesh.MatrixMV.data, 16 * sizeof(float));
						MeshShaderDrawCall.SubMeshMeshletCount = StaticMesh.MeshletCount;
						MeshShaderDrawCall.SubMeshMesletOffset = StaticMesh.MeshletOffset;
						MeshShaderDrawCall.SubMeshVertexOffset = StaticMesh.VertexOffset;
						MeshShaderDrawCall.SubMeshTriangleVertexOffset = StaticMesh.SubMeshTriangleVertexOffset;
						MeshShaderDrawCall.SubMeshTriangleOffset = StaticMesh.SubMeshTriangleOffset;

						static constexpr auto GroupSize = 32;
						_Context.cmd.PushConstant(RhiShaderStageBits::Amp | RhiShaderStageBits::Mesh, &MeshShaderDrawCall, 0u, sizeof(MeshShaderDrawCall));
						const uint32_t DispachtSize = (StaticMesh.MeshletCount + GroupSize - 1) / GroupSize;
						_Context.cmd.DrawMeshTask(DispachtSize, 1u, 1u);
					}
				  },
				}, DrawItem.Data);
		}
	}
}

