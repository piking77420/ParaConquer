#include "Rendering/RenderGraph.hpp"

#include "LowRenderer/CommandList.hpp"
#include "Rendering/RenderPasses/RenderPass.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Rendering/RenderView.hpp"

namespace PC_CORE::Rendering
{


RenderGraph::RenderGraph(Rhi& _Rhi)
	: m_Rhi(_Rhi)
{
	
}

RenderGraph::~RenderGraph()
{

}

void RenderGraph::Build(const RendererPassBuildContext& _RendererPassBuildContext)
{
	m_OutputImage.reset(_RendererPassBuildContext.RHI.CreateTexture());

	m_OutputImage
		->SetTextureType(RhiTexture::Type::Texture2D)
		.SetMemoryUsage(RhiMemoryUsage::Dynamic)
		.SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
		.SetWidth(_RendererPassBuildContext.View.RenderSize.x)
		.SetHeight(_RendererPassBuildContext.View.RenderSize.y)
		.SetTextureUsage(RhiTexture::TextureUsageFlagBits::RenderTarget | RhiTexture::TextureUsageFlagBits::Sampled)
		.SetName("Output Image")
		.Build();

	for (const auto& Nodes : m_Nodes)
	{
		Nodes.BuildFunc(Nodes.RenderPassObject.get(), _RendererPassBuildContext);
	}
}

void RenderGraph::Execute(const RendererPassExecuteContext& _RendererPassExecuteContext)
{
	for (const auto& Nodes : m_Nodes)
	{
		_RendererPassExecuteContext.cmd.BeginDebugLabel(Nodes.GetNameFunc(Nodes.RenderPassObject.get()), Nodes.GetColorFunc(Nodes.RenderPassObject.get()));
		Nodes.ExecuteFunc(Nodes.RenderPassObject.get(), _RendererPassExecuteContext);
		_RendererPassExecuteContext.cmd.EndDebugLabel();
	}
}

}