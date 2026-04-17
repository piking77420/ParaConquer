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

	private:

	};

	REFLECT(DrawPass, RenderPass);
}

#endif // RENDERING_RENDERPASS_DRAWPASS_HEADER
