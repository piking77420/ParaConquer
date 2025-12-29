#pragma once

#include "LowRenderer/RhiTypedef.h"
#include "Reflection/DynamicReflectable.hpp"

namespace PC_CORE
{
	class World;
	class Rhi;
	class CommandList;
}

namespace PC_CORE::Rendering
{
	class RenderPass;
	class View;
	class RenderingWorldData;

	struct RenderGraphContext
	{
		Rhi& Rhi;
		const View& View;
		const RenderingWorldData& RenderingWorldData;
		CommandList& CommandBuffer;
	};

	class PC_CORE_API RenderGraph : public PC_CORE::DynamicReflectable
	{
	public:

		RenderGraph();

		virtual ~RenderGraph();

		IMP_DYNAMIC_REFLECT();

		void Build();

		void Update(const RenderGraphContext& _RenderGraphContext);

		void Execute(const RenderGraphContext& _RenderGraphContext);

	private:
		
	};

	REFLECT(RenderGraph, PC_CORE::DynamicReflectable);

} // PC_CORE::Rendering