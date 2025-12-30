#include "Rendering/RenderGraph.hpp"

#include "LowRenderer/CommandList.hpp"
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

void RenderGraph::Execute(CommandList* _CommandList, const RenderView& _View, const PC_CORE::Rendering::RenderingWorldData& _RenderingWorldData)
{
	for (const auto& Nodes : m_Nodes)
	{
		_CommandList->BeginDebugLabel(Nodes.GetNameFunc(Nodes.RenderPassObject), Nodes.GetColorFunc(Nodes.RenderPassObject));
		Nodes.ExecuteFunc(Nodes.RenderPassObject, _CommandList, _View, _RenderingWorldData);
		_CommandList->EndDebugLabel();
	}
}

}