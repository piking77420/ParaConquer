#pragma once

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

		RenderPassBuildFunc BuildFunc;

		RenderPassExecuteFunc ExecuteFunc;
	};


	class PC_CORE_API RenderGraph : public PC_CORE::DynamicReflectable
	{
	private: 
		template <typename Ret, typename... Args>
		struct RenderPassTrampoline
		{
			template <typename T, Ret(T::* Method)(Args...)>
			static Ret Call(void* obj, Args... args) {
				return (static_cast<T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};

		template <typename Ret>
		struct RenderPassTrampoline<Ret, void>
		{
			template <typename T, Ret(T::* Method)()>
			static Ret Call(void* obj) {
				return (static_cast<T*>(obj)->*Method)();
			}
		};

		template <typename... Args>
		struct RenderPassTrampoline<void, Args...>
		{
			template <typename T, void(T::* Method)(Args...)>
			static void Call(void* obj, Args... args) {
				return (static_cast<T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};

	public:
		RenderGraph();

		~RenderGraph() override;

		IMP_DYNAMIC_REFLECT();

		template <RenderPassT T>
		void AddRenderPass(T* _RenderPassT)
		{
			RenderGraphNode Node;
			Node.RenderPassObject = _RenderPassT;
			Node.GetNameFunc = &RenderPassTrampoline<const char*, void>::Call<T, &T::GetName>;
			Node.BuildFunc = &RenderPassTrampoline<void, const RenderView&>::Call<T, &T::Build>;
			Node.ExecuteFunc = &RenderPassTrampoline<void, PC_CORE::CommandList*, const PC_CORE::Rendering::RenderView&, const PC_CORE::Rendering::RenderingWorldData&>::Call<T, &T::Execute>;
			Nodes.emplace_back(std::move(Node));
		}

		void Build(const RenderView&)
		{
		
		}

		void Execute(CommandList* _CommandList, const RenderView& view)
		{

		}


	private:
		std::vector<RenderGraphNode> Nodes;

		
	};

	REFLECT(RenderGraph, PC_CORE::DynamicReflectable);

} // PC_CORE::Rendering