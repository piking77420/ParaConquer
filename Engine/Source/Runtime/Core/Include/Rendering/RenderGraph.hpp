#pragma once

#include "MetaProgramming.hpp"

#include "LowRenderer/RhiTypedef.h"
#include "Reflection/DynamicReflectable.hpp"
#include "RenderPasses/RenderPass.hpp"
#include "LowRenderer/Rhi.hpp"

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
	class Renderer;



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
		RenderGraph(Rhi& _Rhi);

		~RenderGraph() override;

		IMP_DYNAMIC_REFLECT();

		enum ResourceAccesType : uint8_t
		{
			Write = 1 << 0,
			Read = 1 << 1,
		};

		template <RenderPassT T>
		void AddRenderPass(T* _RenderPassT)
		{
			RenderGraphNode Node;
			Node.RenderPassObject = _RenderPassT;
			Node.GetNameFunc = &MetaProgramming::TrampolineMemberFunc<true, const char*, void>::Call<T, &T::GetName>;
			Node.GetColorFunc = &MetaProgramming::TrampolineMemberFunc <true, std::array<float, 4>, void> ::Call<T, &T::GetColor>;
			Node.BuildFunc = &MetaProgramming::TrampolineMemberFunc<false, void, const RendererPassBuildContext&>::Call<T, &T::Build>;
			Node.ExecuteFunc = &MetaProgramming::TrampolineMemberFunc<true, void, const RendererPassExecuteContext&>::Call<T, &T::Execute>;
			m_Nodes.emplace_back(std::move(Node));
		}

		void Clear()
		{
			m_Nodes.clear();
			m_RenderGraphResources.clear();
		}

		void Build(const RendererPassBuildContext& _RendererPassBuildContext);

		void Execute(const RendererPassExecuteContext& _RendererPassExecuteContext);

		template <RhiResourceType T>
		T& CreateResourceHandle(const char* _Name)
		{
			std::shared_ptr<RhiResource> resource{ nullptr };
			if constexpr (std::is_same_v<T, RhiBuffer>)
			{
				resource = std::shared_ptr<RhiResource>(m_Rhi.CreateBuffer());
			}
			else if constexpr (std::is_same_v<T, RhiTexture>)
			{
				resource = std::shared_ptr<RhiResource>(m_Rhi.CreateTexture());
			}
			else
			{
				static_assert(false);
			}
			assert(resource != nullptr);
			
			m_RenderGraphResources[_Name] = resource;
			resource
				->SetName(_Name);

			return static_cast<T&>(*resource.get());
		}

		template <RhiResourceType T>
		const T& GetResource(const char* _Name)
		{
			return static_cast<const T&>(*m_RenderGraphResources[_Name]);
		}

		const RhiTexture& GetOutPutImage() const
		{
			return *m_OutputImage;
		}


	private:
		Rhi& m_Rhi;

		std::unordered_map<std::string_view, std::shared_ptr<RhiResource>> m_RenderGraphResources;

		std::unique_ptr<RhiTexture> m_OutputImage;

		std::vector<RenderGraphNode> m_Nodes;

	};


} // PC_CORE::Rendering