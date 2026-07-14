#include <tuple>

#include <DebugView/DebugShapeDraw.hpp>
#include <Rendering/RenderGraph.hpp>
#include <Rendering/RenderView.hpp>
#include <Rendering/Renderer.hpp>

namespace PC_EDITOR::DebugView
{
	namespace Rendering = PC_CORE::Rendering;

	DebugShapeDraw::DebugShapeDraw()
	{
		DYNAMIC_REFLECT_INIT
	}

	DebugShapeDraw::~DebugShapeDraw()
	{

	}

	void DebugView::DebugShapeDraw::Build(const Rendering::RendererPassBuildContext& _RendererPassBuildContext)
	{
		PC_CORE::RhiTexture& outPutImage = _RendererPassBuildContext.RenderGraph.GetOutPutImage();
		PC_CORE::RhiTexture* DepthBuffer = _RendererPassBuildContext.RenderGraph.GetResource<PC_CORE::RhiTexture>("Depth Buffer");
		if (!DepthBuffer)
		{
			PC_LOGERROR("Failed to find depth Buffer");
			return;
		}

		m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
		m_FrameBuffer
			->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetAttachement(&outPutImage)
			.SetDepthAttachment(DepthBuffer)
			.SetRenderPass(_RendererPassBuildContext.Renderer.LoadHdrColorLoadStoreDepth.get())
			.SetName("DebugShapeDraw Framebuffer")
			.Build();

		for (size_t i = 0; i < static_cast<size_t>(PC_CORE::DebugDrawContext::PrimitiveType::Count); i++)
		{
			const auto& PrimitiveData = _RendererPassBuildContext.Renderer.m_DebugPrimitiveBuffer[i];
			auto& InstanceBuffer = PrimitiveData.InstanceBuffer;
			
			auto& Descriptor = m_DescriptorSets[InstanceBuffer->GetName()];

			Descriptor.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
			Descriptor
				->BindUniformBuffer(RhiShaderStageBits::Vertex, 0, _RendererPassBuildContext.View.UniformBuffer.get())
				.BindShaderStorageBuffer(RhiShaderStageBits::Vertex, 1, InstanceBuffer.get())
				.SetName("DebugShapeDraw " + PC_CORE::DebugDrawContext::PrimitiveTypeToString(static_cast<PC_CORE::DebugDrawContext::PrimitiveType>(i)))
				.Build();
		}


		m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DescriptorSet
			->BindUniformBuffer(RhiShaderStageBits::Vertex, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.SetName("DebugShapeDraw Base")
			.Build();
		
	}

	void DebugView::DebugShapeDraw::Execute(const Rendering::RendererPassExecuteContext & _RendererPassExecuteContext)
	{
		using namespace PC_CORE;

		CommandList& cmd = _RendererPassExecuteContext.cmd;

		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = _RendererPassExecuteContext.Renderer.LoadHdrColorLoadStoreDepth.get(),
			.FrameBuffer = m_FrameBuffer.get(),
			.RenderOffSet = {0, 0},
			.Extent = {m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight()},
		};
		cmd.BeginRenderPass(beginRenderPassInfo);
		ViewportInfo viewPort(beginRenderPassInfo.Extent);
		cmd.SetViewPort(viewPort);
		
		for (auto& DebugDrawInstanced: _RendererPassExecuteContext.Renderer.DebugDrawList)
		{
			if (std::holds_alternative<PC_CORE::Rendering::DrawDebugInstanced>(DebugDrawInstanced.Data))
			{
				const PC_CORE::Rendering::DrawDebugInstanced& DrawDebugInstanced = std::get<PC_CORE::Rendering::DrawDebugInstanced>(DebugDrawInstanced.Data);
				auto Descriptor = m_DescriptorSets.find(DrawDebugInstanced.InstanceBuffer->GetName());
				if (Descriptor != m_DescriptorSets.end())
				{
					// bind programm etc
					cmd.BindDescriptorSet(Descriptor->second.get(), 0);

					if (DrawDebugInstanced.isWired)
					{
						cmd.SetPrimitiveTopology(RhiGraphicPipeline::PrimitiveTopology::PrimitiveTopologyLineList);
						cmd.SetLineWidth(1.f);
					}
					else
					{
						cmd.SetPrimitiveTopology(RhiGraphicPipeline::PrimitiveTopologyTriangleList);
					}

					CommandList::DrawBuffers drawBuffer;
					drawBuffer
						.PushVertexBuffer(
							*DrawDebugInstanced.VertexBuffer,
							0ull)
						.SetIndexBuffer(
							*DrawDebugInstanced.IndexBuffer,
							0ull,
							DrawDebugInstanced.IndexFormat
						);
					cmd.BindDrawBuffers(drawBuffer);
					cmd.DrawIndexed(DrawDebugInstanced.IndexCount, DrawDebugInstanced.InstanceCount, 0, 0, 0);
				}
			}
			
			else if (std::holds_alternative<PC_CORE::Rendering::DrawDebug>(DebugDrawInstanced.Data))
			{
				const PC_CORE::Rendering::DrawDebug& DrawDebug = std::get<PC_CORE::Rendering::DrawDebug>(DebugDrawInstanced.Data);
				cmd.BindDescriptorSet(m_DescriptorSet.get(), 0);

				if (DrawDebug.isWired)
				{
					cmd.SetPrimitiveTopology(RhiGraphicPipeline::PrimitiveTopology::PrimitiveTopologyLineList);
					cmd.SetLineWidth(1.f);
				}
				else
				{
					cmd.SetPrimitiveTopology(RhiGraphicPipeline::PrimitiveTopologyTriangleList);
				}
				cmd.PushConstant(RhiShaderStageBits::Vertex, &DrawDebug.VP, 0, sizeof(DrawDebug.VP));
				static constexpr size_t FrustumIndexCount = 24;
				assert(DrawDebug.IndexCount == FrustumIndexCount);
				cmd.Draw(DrawDebug.IndexCount, 1, 0, 0);
			}
		}
		

		cmd.EndRenderPass();
	}

} // PC_EDITOR::DebugView::
