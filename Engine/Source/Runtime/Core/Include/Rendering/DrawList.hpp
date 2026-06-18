#ifndef RENDERING_DRAWLIST
#define RENDERING_DRAWLIST

#include <variant>
#include <LowRenderer/RhiBuffer.h>
#include <LowRenderer/RhiShaderProgram.hpp>
#include <LowRenderer/RhiDescriptorSet.hpp>
#include <DebugHelper/DebugDrawContext.hpp>

namespace PC_CORE::Rendering
{
	struct DrawStaticMeshTriangle
	{
		const RhiShaderProgram* ShaderProgram{ nullptr };
		const RhiDescriptorSet* MaterialDescriptor{ nullptr };
		const RhiBuffer* VertexBuffer{ nullptr };
		const RhiBuffer* IndexBuffer{ nullptr };
		RhiBuffer::IndexFormat IndexFormat{};
		uint32_t MaterialDescriptorOffset{0u};

		uint32_t VertexOffset{ 0u };
		uint32_t IndexCount{ 0u };
		uint32_t IndexOffset{ 0u };
	};

	struct DrawStaticMeshMeshlet
	{
		const RhiShaderProgram* ShaderProgram{ nullptr };
		const RhiDescriptorSet* MaterialDescriptor{ nullptr };
		const RhiDescriptorSet* MeshletDescriptor{ nullptr };
		const RhiDescriptorSet* MeshletBoundDescriptor{ nullptr };
		uint32_t MaterialDescriptorOffset{ 0u };

		uint32_t VertexOffset{ 0u };
		uint32_t MeshletOffset{ 0u };
		uint32_t MeshletCount{ 0u };
		uint32_t SubMeshTriangleVertexOffset{ 0u };
		uint32_t SubMeshTriangleOffset{ 0u };
	};

	struct DrawDebug
	{
		const RhiShaderProgram* ShaderProgram{ nullptr };
		Tbx::Matrix4x4f VP;
		bool isWired = false;
		uint32_t IndexCount{ 0 };
	};

	struct DrawDebugInstanced
	{
		const RhiShaderProgram* ShaderProgram{ nullptr };
		const RhiBuffer* VertexBuffer{ nullptr };
		const RhiBuffer* IndexBuffer{ nullptr };
		const RhiBuffer* InstanceBuffer{ nullptr };
		RhiBuffer::IndexFormat IndexFormat;
		bool isWired = false;

		uint32_t IndexCount{ 0 };
		uint32_t InstanceCount{ 0 };
	};

	struct DrawSkyBox
	{
		Tbx::Matrix4x4f ViewProjectionCorrectedMatrix;
	};


	struct DrawItem
	{
		uint64_t SortKey;
		size_t InstanceIndex;
		std::variant <
			DrawStaticMeshTriangle,
			DrawStaticMeshMeshlet,
			DrawSkyBox,
			DrawDebug,
			DrawDebugInstanced
		>Data;
	};

	// https://en.cppreference.com/w/cpp/utility/variant/visit2.html
	// helper type for the visitor #4
	template<class... Ts>
	struct overloaded : Ts... { using Ts::operator()...; };
	// explicit deduction guide (not needed as of C++20)
	template<class... Ts>
	overloaded(Ts...) -> overloaded<Ts...>;

	class DrawList  
	{
	public:
		DrawList() = default;
		~DrawList() = default;

		void PushBack(const DrawItem& DrawItem)
		{
			m_items.push_back(DrawItem);
		}

		void PushBack(DrawItem&& DrawItem)
		{
			m_items.push_back(std::move(DrawItem));
		}

		DrawItem& EmplaceBack(const DrawItem& DrawItem)
		{
			m_items.emplace_back(DrawItem);
		}

		DrawItem& EmplaceBack(DrawItem&& DrawItem)
		{
			return m_items.emplace_back(std::move(DrawItem));
		}

		DrawItem& EmplaceBack()
		{
			return m_items.emplace_back();
		}

		template<typename... Args>
		DrawItem& EmplaceBack(Args&&... args)
		{
			return m_items.emplace_back(std::forward<Args>(args)...);
		}

		template<typename Comp>
		void Sort(Comp&& comp)
		{
			std::ranges::sort(m_items, std::forward<Comp>(comp));
		}

		template<typename Comp, typename Proj>
		void Sort(Comp&& comp, Proj&& proj)
		{
			std::ranges::sort(m_items,
				std::forward<Comp>(comp),
				std::forward<Proj>(proj));
		}

		std::vector<DrawItem>& GetDrawItem()
		{ 
			return m_items;
		}

		const std::vector<DrawItem>& GetDrawItem() const
		{
			return m_items;
		}

		void Clear()
		{
			return m_items.clear();
		}

		auto begin() { return m_items.begin(); }

		auto end() { return m_items.end(); }

		auto begin() const { return m_items.begin(); }

		auto end()   const { return m_items.end(); }

		size_t size() const { return m_items.size(); }

		bool empty() const { return m_items.empty(); }

		void Reserve(size_t Size)
		{
			m_items.reserve(Size);
		}

		void Resize(size_t Size)
		{
			m_items.resize(Size);
		}

	private:
		std::vector<DrawItem> m_items;

	};

	
} // namespace PC_CORE::Rendering

#endif // RENDERING_DRAWLIST
