#pragma once

#include "MetaProgramming.hpp"

#include "LowRenderer/RhiTypedef.h"
#include "Reflection/DynamicReflectable.hpp"
#include "RenderPasses/RenderPass.hpp"

namespace PC_CORE
{
	class World;
	class Rhi;
	class CommandList;
}

namespace PC_CORE::Rendering
{
	class RenderView;
	class RenderingWorldData;



	struct RenderGraphNode
	{
		void* RenderPassObject;

		RenderPassGetNameFunc GetNameFunc;

		RenderPassGetColorFunc GetColorFunc;

		RenderPassBuildFunc BuildFunc;

		RenderPassExecuteFunc ExecuteFunc;
	};


	class PC_CORE_API RenderGraph : public PC_CORE::DynamicReflectable
	{
	private: 
	

	public:
		RenderGraph();

		~RenderGraph() override;

		IMP_DYNAMIC_REFLECT();

		template <RenderPassT T>
		void AddRenderPass(T* _RenderPassT)
		{
			RenderGraphNode Node;
			Node.RenderPassObject = _RenderPassT;
			Node.GetNameFunc = &MetaProgramming::TrampolineMemberFunc<true, const char*, void>::Call<T, &T::GetName>;
			Node.GetColorFunc = &MetaProgramming::TrampolineMemberFunc <true, std::array<float, 4>, void> ::Call<T, &T::GetColor>;
			Node.BuildFunc = &MetaProgramming::TrampolineMemberFunc<false, void, const RenderView&>::Call<T, &T::Build>;
			Node.ExecuteFunc = &MetaProgramming::TrampolineMemberFunc<true, void, PC_CORE::CommandList*, const PC_CORE::Rendering::RenderView&, const PC_CORE::Rendering::RenderingWorldData&>::Call<T, &T::Execute>;
			m_Nodes.emplace_back(std::move(Node));
		}

		void Clear()
		{
			m_Nodes.clear();
		}

		void Build(const RenderView&)
		{
			
		}

		void Execute(CommandList* _CommandList, const RenderView& _View, const PC_CORE::Rendering::RenderingWorldData& _RenderingWorldData);

	private:
		std::vector<RenderGraphNode> m_Nodes;

	};

	REFLECT(RenderGraph, PC_CORE::DynamicReflectable);

} // PC_CORE::Rendering