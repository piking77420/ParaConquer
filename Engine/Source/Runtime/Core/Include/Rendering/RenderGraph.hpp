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
	class RenderGraph;

	template <typename T>
	concept RenderPassT = std::is_base_of_v<RenderPass, T>;

	struct RenderGraphContext
	{
		const View& View;
		const RenderGraph& RenderGraph;
		const RenderingWorldData& RenderingWorldData;
		CommandList& CommandBuffer;
	};

	class RenderGraph : public PC_CORE::DynamicReflectable
	{
	public:

		RenderGraph();

		virtual ~RenderGraph();

		IMP_DYNAMIC_REFLECT();

		RenderGraph& Clear();

		template <RenderPassT T>
		RenderGraph& Add()
		{
			m_RenderPasses.emplace_back(std::make_unique<T>());
			return *this;
		}

		template <RenderPassT T>
		RenderGraph& Remove()
		{
			constexpr TypeId id = GetTypeKey<T>();

			for (auto it = m_RenderPasses.begin(); it != m_RenderPasses.end(); it++)
			{
				if (it->get().GetTypeKey() == id)
					m_RenderPasses.erase(it);
			}
			return *this;
		}

		template <RenderPassT T>
		RenderPass& Get() const
		{
			constexpr TypeId id = GetTypeKey<T>();

			auto it = std::ranges::find_if(m_RenderPasses, [id](const std::unique_ptr<RenderPass>& renderPassPtr)
				{
					return renderPassPtr->GetTypeKey() == id;
				});

			return *it;
		}

	

	private:
		std::vector<std::unique_ptr<RenderPass>> m_RenderPasses;
		
	};



} // PC_CORE::Rendering