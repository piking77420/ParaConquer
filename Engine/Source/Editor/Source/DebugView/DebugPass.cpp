#include <DebugView/DebugPass.hpp>
#include <Rendering/RenderGraph.hpp>
#include <Rendering/RenderView.hpp>
#include <Rendering/Renderer.hpp>

namespace PC_EDITOR::DebugView
{
using namespace PC_CORE;
using namespace PC_CORE::Rendering;

DebugPass::DebugPass()
{
	DYNAMIC_REFLECT_INIT;
}

DebugPass::DebugPass(const std::string& _Name,
	const std::array<float, 4>& _GpuColor,
	std::unique_ptr<PC_CORE::RhiShaderProgram>*& _ShaderProgramTriangle,
	std::unique_ptr<PC_CORE::RhiShaderProgram>*& _ShaderProgramMeshlet)
	: DrawPass()
	, m_Name(_Name)
	, m_GpuDebugerColor(_GpuColor)
	, m_ShaderProgramTriangle(_ShaderProgramTriangle)
	, m_ShaderProgramMeshlet(_ShaderProgramMeshlet)
{
	DYNAMIC_REFLECT_INIT
}

DebugPass::~DebugPass() = default;

void DebugPass::Build(const PC_CORE::Rendering::RendererPassBuildContext& _RendererPassBuildContext)
{
	RhiTexture& outPutImage = _RendererPassBuildContext.RenderGraph.GetOutPutImage();

	RhiTexture& DepthBuffer = _RendererPassBuildContext.RenderGraph.CreateResourceHandle<RhiTexture>("Depth Buffer");
	DepthBuffer
		.SetTextureType(RhiTexture::Type::Texture2D)
		.SetMemoryUsage(RhiMemoryUsage::StaticGPU)
		.SetRhiFormat(RhiFormat::D24UnormS8Uint)
		.SetWidth(_RendererPassBuildContext.View.RenderSize.x)
		.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
		.SetTextureUsage(RhiTexture::TextureUsageFlagBits::DepthStencil)
		.SetSamples(1)
		.SetName(m_Name + "Depth Image")
		.Build();

	m_FrameBuffer.reset(_RendererPassBuildContext.RHI.CreateFrameBuffer());
	m_FrameBuffer
		->SetWidth(_RendererPassBuildContext.View.RenderSize.x)
		.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
		.SetAttachments(&outPutImage)
		.SetDepthAttachments(&DepthBuffer)
		.SetRenderPass(_RendererPassBuildContext.Renderer.colorLinearPassDepth.get())
		.SetName(m_Name + "Framebuffer")
		.Build();

	m_DescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
	m_DescriptorSet
		->BindUniformBuffer(RhiShaderStageBits::Vertex, 0, _RendererPassBuildContext.View.UniformBuffer.get())
		.BindShaderStorageBuffer(RhiShaderStageBits::Vertex, 1, _RendererPassBuildContext.Renderer.InstanceBuffer.get())
		.SetName(m_Name + "Pass Scene Set")
		.Build();

	m_MeshShaderDescriptorSet.reset(_RendererPassBuildContext.RHI.CreateDescriptorSet());
	m_MeshShaderDescriptorSet
		->BindUniformBuffer(RhiShaderStageBits::Mesh, 0, _RendererPassBuildContext.View.UniformBuffer.get())
		.BindShaderStorageBuffer(RhiShaderStageBits::Mesh, 1, _RendererPassBuildContext.Renderer.InstanceBuffer.get())
		.SetName(m_Name + "Pass Scene Mesh Shader Set")
		.Build();
	
	m_OnMeshDrawTriangle = [&](const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const Rendering::DrawStaticMeshTriangle& StaticMesh)
		{
			if (_Context.cmd.BindProgram(*m_ShaderProgramTriangle->get()))
			{
				_Context.cmd.BindDescriptorSet(m_DescriptorSet.get(), 0);
			}
		};

	m_OnMeshDrawMeshlet = [&](const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const Rendering::DrawStaticMeshMeshlet& StaticMesh)
		{
			if (_Context.cmd.BindProgram(*m_ShaderProgramMeshlet->get()))
			{
				_Context.cmd.BindDescriptorSet(m_MeshShaderDescriptorSet.get(), 0);
			}
		};
}

void DebugPass::Execute(const PC_CORE::Rendering::RendererPassExecuteContext & _RendererPassExecuteContext) const
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Rendering)

	CommandList& cmd = _RendererPassExecuteContext.cmd;

	std::array<float, 4> Color = GetColor();

	const BeginRenderPassInfo beginRenderPassInfo =
	{
		.RenderPass = _RendererPassExecuteContext.Renderer.colorLinearPassDepth.get(),
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
	cmd.EndRenderPass();
}

} // namespace PC_EDITOR::DebugView