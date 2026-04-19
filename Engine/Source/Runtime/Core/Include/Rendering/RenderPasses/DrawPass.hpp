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
	class PC_CORE_API DrawPass : public RenderPass
	{
	public:
		DrawPass();
		virtual ~DrawPass();

	protected:
		struct alignas(16) ModelPushConstant
		{
			Gpu::mat4 ModelView;
			Gpu::mat4 NormalInvMatrixView;
		};

		bool m_DrawTransperant = false;

		std::function<void(const Rendering::DrawStaticMeshTriangle&, PC_CORE::CommandList& _Cmd)> m_OnMeshDrawTriangle{};

		std::function<void(const Rendering::DrawStaticMeshMeshlet&, PC_CORE::CommandList& _Cmd)> m_OnMeshDrawMeshlet{};

		const RhiShaderProgram* m_LastShaderProgram{ nullptr };

		const RhiDescriptorSet* m_LastMaterialDescriptor{ nullptr };

		void ProceedDrawList(const DrawList& _DrawList, PC_CORE::CommandList& _Cmd) const;
	private:

	};

	REFLECT(DrawPass, RenderPass);
}

#endif // RENDERING_RENDERPASS_DRAWPASS_HEADER
