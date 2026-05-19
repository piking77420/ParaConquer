#ifndef RENDERING_RENDERPASS_DRAWPASS_HEADER
#define RENDERING_RENDERPASS_DRAWPASS_HEADER

#include <functional>
#include <Rendering/RenderPasses/RenderPass.hpp>
#include <Rendering/RenderingTypedef.h>

namespace PC_CORE::Rendering
{
	class Renderer;
	struct StaticMeshComponentData;
	struct DrawStaticMeshTriangle;
	struct DrawStaticMeshMeshlet;
}

namespace PC_CORE::Rendering::Pass 
{
	using OnMeshDrawTriangleSignature = std::function<void(const PC_CORE::Rendering::RendererPassExecuteContext&, const Rendering::DrawStaticMeshTriangle&)>;
	using OnMeshDrawMeshletSignature = std::function<void(const PC_CORE::Rendering::RendererPassExecuteContext&, const Rendering::DrawStaticMeshMeshlet&)>;

	class PC_CORE_API DrawPass : public RenderPass
	{
	public:
		static constexpr uint32_t GroupSize = 32u;

		DrawPass();

		DrawPass(OnMeshDrawTriangleSignature _OnMeshDrawTriangleSignature, OnMeshDrawMeshletSignature _OnMeshDrawMeshlet);

		virtual ~DrawPass();

	protected:
		struct MeshShaderDrawCall
		{
			uint32_t RenderInstanceID;
			uint32_t SubMeshMeshletCount;
			uint32_t SubMeshMesletOffset;
			uint32_t SubMeshVertexOffset;
			uint32_t SubMeshTriangleVertexOffset;
			uint32_t SubMeshTriangleOffset;
		};

		bool m_DrawTransperant = false;

		bool m_OnlyBindMeshletBound = false;

		OnMeshDrawTriangleSignature m_OnMeshDrawTriangle;

		OnMeshDrawMeshletSignature m_OnMeshDrawMeshlet{};

		void ProceedDrawList(const PC_CORE::Rendering::RendererPassExecuteContext& _Context, const DrawList& _DrawList) const;

		mutable const RhiDescriptorSet* m_LastMaterialDescriptor{ nullptr };
		mutable const RhiDescriptorSet* m_LastMeshletDescritptor{ nullptr };
	private:

	};

	REFLECT(DrawPass, RenderPass);
}

#endif // RENDERING_RENDERPASS_DRAWPASS_HEADER
