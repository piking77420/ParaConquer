#include "Rendering/RenderGraph.hpp"

#include "Rendering/RenderPasses/RenderPass.hpp"

namespace PC_CORE::Rendering
{

RenderGraph::RenderGraph()
{
	DYNAMIC_REFLECT_INIT;
}

RenderGraph::~RenderGraph()
{

}

void RenderGraph::Build()
{

}

void RenderGraph::Update(const RenderGraphContext& _RenderGraphContext)
{
}

void RenderGraph::Execute(const RenderGraphContext& _RenderGraphContext)
{

}


}