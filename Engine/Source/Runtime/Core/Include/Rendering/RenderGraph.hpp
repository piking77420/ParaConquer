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

		RenderPassBuildFunc BuildFunc;

		RenderPassExecuteFunc ExecuteFunc;
	};


	class PC_CORE_API RenderGraph : public PC_CORE::DynamicReflectable
	{
	private: 
		template <bool Cst ,typename Ret, typename... Args>
		struct RenderPassTrampoline
		{
			template <typename T, Ret(T::* Method)(Args...)>
			static Ret Call(void* obj, Args... args) {
				return (static_cast<T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};

		// None const 
		template <typename Ret>
		struct RenderPassTrampoline<false, Ret, void>
		{
			template <typename T, Ret(T::* Method)()>
			static Ret Call(void* obj) {
				return (static_cast<T*>(obj)->*Method)();
			}
		};

		template <typename... Args>
		struct RenderPassTrampoline<false, void, Args...>
		{
			template <typename T, void(T::* Method)(Args...)>
			static void Call(void* obj, Args... args) {
				return (static_cast<T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};

		// Const
		template <typename Ret>
		struct RenderPassTrampoline<true, Ret, void>
		{
			template <typename T, Ret(T::* Method)() const>
			static Ret Call(void* obj) {
				return (static_cast<T*>(obj)->*Method)();
			}
		};

		template <typename... Args>
		struct RenderPassTrampoline<true, void, Args...>
		{
			template <typename T, void(T::* Method)(Args...) const>
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
			Node.GetNameFunc = &RenderPassTrampoline<true ,const char*, void>::Call<T, &T::GetName>;
			Node.BuildFunc = &RenderPassTrampoline<false, void, const RenderView&>::Call<T, &T::Build>;
			Node.ExecuteFunc = &RenderPassTrampoline<true, void, PC_CORE::CommandList*, const PC_CORE::Rendering::RenderView&, const PC_CORE::Rendering::RenderingWorldData&>::Call<T, &T::Execute>;
			m_Nodes.emplace_back(std::move(Node));
		}

		void Clear()
		{
			m_Nodes.clear();
		}

		void Build(const RenderView&)
		{
			
		}

		void Execute(CommandList* _CommandList, const RenderView& view)
		{

		}


	private:
		std::vector<RenderGraphNode> m_Nodes;

	};

	REFLECT(RenderGraph, PC_CORE::DynamicReflectable);

} // PC_CORE::Rendering