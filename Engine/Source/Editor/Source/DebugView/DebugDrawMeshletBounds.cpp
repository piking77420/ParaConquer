#include <DebugView/DebugDrawMeshletBounds.hpp>
#include <Rendering/RenderGraph.hpp>
#include <Rendering/RenderView.hpp>
#include <Rendering/Renderer.hpp>

namespace PC_EDITOR::DebugView
{
	using namespace PC_CORE;
	using namespace PC_CORE::Rendering;

	DebugDrawMeshletBounds::DebugDrawMeshletBounds()
	{
		m_OnlyBindMeshletBound = true;
	}

	DebugDrawMeshletBounds::~DebugDrawMeshletBounds()
	{

	}

	void DebugDrawMeshletBounds::Build(const PC_CORE::Rendering::RendererPassBuildContext& _RendererPassBuildContext)
	{
		RhiTexture& outPutImage = _RendererPassBuildContext.RenderGraph.GetOutPutImage();

		RhiTexture* DepthBuffer = _RendererPassBuildContext.RenderGraph.GetResource<RhiTexture>("Depth Buffer");

		if (!DepthBuffer)
		{
			PC_LOGERROR("Can't find dpeht buffer");
			return;
		}

		m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
		m_FrameBuffer
			->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
			.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
			.SetAttachement(&outPutImage)
			.SetDepthAttachment(DepthBuffer)
			.SetRenderPass(_RendererPassBuildContext.Renderer.LinearClearColorClearStoreDepth.get())
			.SetName("DebugDrawMeshletBounds Framebuffer")
			.Build();

		const auto& WireSphereDebugPrimitive = _RendererPassBuildContext.Renderer.m_DebugPrimitiveBuffer[static_cast<size_t>(DebugDrawContext::PrimitiveType::WireSphere)];

		m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
		m_DescriptorSet
			->BindUniformBuffer(RhiShaderStageBits::Amp | RhiShaderStageBits::Mesh, 0, _RendererPassBuildContext.View.UniformBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Amp | RhiShaderStageBits::Mesh, 1, _RendererPassBuildContext.Renderer.InstanceBuffer.get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 2, WireSphereDebugPrimitive.VertexBuffer.Get())
			.BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 3, WireSphereDebugPrimitive.IndexBuffer.Get())
			.SetName("DebugDrawMeshletBounds Pass Scene Set")
			.Build();

		m_OnMeshDrawTriangle = [&](const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const Rendering::DrawStaticMeshTriangle& StaticMesh)
			{
				assert(false && "Shoudl not be used in regular pipeline");
			};

		m_OnMeshDrawMeshlet = [&](const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const Rendering::DrawStaticMeshMeshlet& StaticMesh)
			{
				if (_Context.Renderer.DrawDebugMeshletBound)
				{
					_Context.cmd.BindProgram(*_Context.Renderer.DrawDebugMeshletBound);
					_Context.cmd.BindDescriptorSet(m_DescriptorSet.get(), 0);
				}
			};
	}

	void DebugDrawMeshletBounds::Execute(const PC_CORE::Rendering::RendererPassExecuteContext & _RendererPassExecuteContext)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering)

		CommandList& cmd = _RendererPassExecuteContext.cmd;

		std::array<float, 4> Color = GetColor();

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
		cmd.SetLineWidth(1.0f);
		ProceedDrawList(_RendererPassExecuteContext, _RendererPassExecuteContext.Renderer.OpaqueList);
		ProceedDrawList(_RendererPassExecuteContext, _RendererPassExecuteContext.Renderer.TransparentList);
		cmd.EndRenderPass();
	}
} // PC_EDITOR::DebugView