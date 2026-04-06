#ifndef RENDERING_RENDERPASS_DRAWPASS_HEADER
#define RENDERING_RENDERPASS_DRAWPASS_HEADER

#include <Rendering/RenderPasses/RenderPass.hpp>

namespace PC_CORE::Rendering
{
	class Renderer;
	struct StaticMeshComponentData;
}

namespace PC_CORE::Rendering::Pass 
{
	class DrawPass : public RenderPass
	{
	public:
		DrawPass();
		virtual ~DrawPass();

	protected:
		bool m_DrawTransperant = false;

		size_t PickLodCount(const std::vector<double>& LodThreshold, const MotionCore::Aabb<double>& AABBW, const Tbx::Vector3d& CameraPos, double FovRad) const;

		void DrawStaticMesh(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj, const RhiDescriptorSet* RenderPassSet, size_t FirstSet) const;

	private:
		mutable std::vector<std::pair<double, uint32_t>> m_TransparentSubMeshDistanceV;

		void DrawStaticMeshTriangle(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj, const RhiDescriptorSet* RenderPassSet, size_t FirstSet) const;

		void DrawStaticMeshMeshlet(const RendererPassExecuteContext& _RendererPassExecuteContext, const Rendering::StaticMeshComponentData& _DrawObj, const RhiDescriptorSet* RenderPassSet, size_t FirstSet) const;
	};

	REFLECT(DrawPass, RenderPass);
}

#endif // RENDERING_RENDERPASS_DRAWPASS_HEADER
