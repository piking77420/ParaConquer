#include "Rendering/RenderGraph.hpp"

PC_CORE::Rendering::RenderGraph::RenderGraph()
{
	DYNAMIC_REFLECT_INIT;
}

PC_CORE::Rendering::RenderGraph::~RenderGraph()
{
	
}

PC_CORE::Rendering::RenderGraph& PC_CORE::Rendering::RenderGraph::Clear()
{
	m_RenderPasses.clear();
}
