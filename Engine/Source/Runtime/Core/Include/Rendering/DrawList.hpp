#ifndef RENDERING_DRAWLIST
#define RENDERING_DRAWLIST

#include <variant>
#include <LowRenderer/RhiBuffer.h>
#include <LowRenderer/RhiShaderProgram.hpp>
#include <LowRenderer/RhiDescriptorSet.hpp>

namespace PC_CORE::Rendering
{
	struct DrawStaticMeshTriangle
	{
		const RhiShaderProgram* ShaderProgram{ nullptr };
		const RhiDescriptorSet* MaterialDescriptor{ nullptr };
		const RhiBuffer* VertexBuffer{ nullptr };
		const RhiBuffer* IndexBuffer{ nullptr };

		uint32_t VertexOffset{ 0u };
		uint32_t IndexCount{ 0u };
		uint32_t IndexOffset{ 0u };

		double DitanceAABBToCam;
	};

	struct DrawStaticMeshMeshlet
	{
		const RhiShaderProgram* ShaderProgram{ nullptr };
		const RhiDescriptorSet* MaterialDescriptor{ nullptr };
		const RhiBuffer* VertexBuffer{ nullptr };
		const RhiBuffer* MeshletBuffer{ nullptr };
		const RhiBuffer* MeshletAABBBuffer{ nullptr };
		const RhiBuffer* MeshletTriangleBuffer{ nullptr };
		const RhiBuffer* MeshletVertexTriangleIndexBuffer{ nullptr }; 
	};

	// sortkey and data
	using DrawItem = std::variant<DrawStaticMeshTriangle, DrawStaticMeshMeshlet>;

	class DrawList
	{
	public:
		DrawList() = default;
		~DrawList() = default;

		void AddItem(const DrawItem& DrawItem)
		{
			m_items.push_back(DrawItem);
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

	private:
		std::vector<DrawItem> m_items;

	};

	
} // namespace PC_CORE::Rendering

#endif // RENDERING_DRAWLIST
