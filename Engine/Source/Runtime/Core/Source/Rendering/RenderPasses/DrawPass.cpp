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
#include <LowRenderer/RhiDescriptorSet.hpp>
#include <LowRenderer/RhiFrameBuffer.hpp>

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

	void DrawPass::ProceedDrawList(const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const DrawList& _DrawList)
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

						const uint32_t RenderInstanceID = DrawItem.InstanceIndex;
						_Context.cmd.PushConstant(RhiShaderStageBits::Vertex, &RenderInstanceID, 0u, sizeof(RenderInstanceID));
						_Context.cmd.BindDrawBuffers(drawBuffer);
						_Context.cmd.DrawIndexed(StaticMesh.IndexCount, 1, StaticMesh.IndexOffset, StaticMesh.VertexOffset, 0);
					}

				},
				[&](const Rendering::DrawStaticMeshMeshlet& StaticMesh) {
					if (m_OnMeshDrawMeshlet)
					{
						
						m_OnMeshDrawMeshlet(_Context, StaticMesh);

						if (StaticMesh.MeshletDescriptor && m_LastMeshletDescritptor != StaticMesh.MeshletDescriptor)
						{
							if (m_OnlyBindMeshletBound)
							{
 								m_LastMeshletDescritptor = StaticMesh.MeshletBoundDescriptor;
								_Context.cmd.BindDescriptorSet(StaticMesh.MeshletBoundDescriptor, 1);
							}
							else
							{
								m_LastMeshletDescritptor = StaticMesh.MeshletDescriptor;
								_Context.cmd.BindDescriptorSet(StaticMesh.MeshletDescriptor, 3);
							}
							
						}
						MeshShaderDrawCall MeshShaderDrawCall;
						MeshShaderDrawCall.RenderInstanceID = DrawItem.InstanceIndex;
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
				[&](const DrawSkyBox& DrawSkyBox) {

					// Dirty make a proper Pass
					struct DrawSkyBoxPipelineData
					{
						PipelineCacheID Id;
						PipelineCache::ModuleEntryList List;
						RhiGraphicPipeline::Descriptor GraphicPipelineDescriptor;
					};

					static DrawSkyBoxPipelineData DrawSkyBoxPipelineData;

					DrawSkyBoxPipelineData.List.Reserve(2);
					auto& Vertex = DrawSkyBoxPipelineData.List.Next();
					Vertex
						.SetSourcePath("/Shaders/CubeMap.vs.hlsl")
						.SetShaderFeaturesFlag(ShaderFeature::UseUV | ShaderFeature::SkyboxForceFarDepth);

					auto& Frag = DrawSkyBoxPipelineData.List.Next();
					Frag
						.SetSourcePath("/Shaders/Skybox.ps.hlsl")
						.SetShaderFeaturesFlag(ShaderFeature::UseUV);

					DrawSkyBoxPipelineData.GraphicPipelineDescriptor.SetDepthTest(true)
						.SetDepthCompareOp(CompareOp::LessOrEqual)
						.SetDepthWrite(false)
						.SetRenderPass(*_Context.Renderer.forwardPass);

					PipelineCache::GraphicPipelineQueryResult Result = _Context.PipelineCache.CreateOrGetGraphicPipelineCache(
						&DrawSkyBoxPipelineData.Id,
						"Skybox",
						DrawSkyBoxPipelineData.List, 
						DrawSkyBoxPipelineData.GraphicPipelineDescriptor);

					if (Result && _Context.cmd.BindRhiPipeline(*Result))
					{
						_Context.cmd.BindDescriptorSet(_Context.Renderer.SkyBoxDescriptorSet.get(), 0);
						_Context.cmd.PushConstant(RhiShaderStageBits::Vertex, &DrawSkyBox.ViewProjectionCorrectedMatrix, 0, sizeof(DrawSkyBox.ViewProjectionCorrectedMatrix));
						_Context.cmd.Draw(36, 1, 0, 0);
					}

				},
				[&](auto&&) 
				{

				},
				}, DrawItem.Data);
		}
	}
}

