#include <Rendering/RenderPasses/DrawPass.hpp>

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderSystem.hpp"
#include "Rendering/RenderView.hpp"
#include <LowRenderer/CommandList.hpp>
#include <Rendering/RenderView.hpp>
#include <Resources/StaticMesh.hpp>
#include <World/StaticMeshComponent.hpp>

namespace PC_CORE::Rendering::Pass
{
	DrawPass::DrawPass()
	{
		DYNAMIC_REFLECT_INIT
	}

	DrawPass::~DrawPass() = default;

	void DrawPass::ProceedDrawList(const DrawList& _DrawList, PC_CORE::CommandList& _Cmd) const
	{
		for (const auto& DrawItem : _DrawList)
		{
			std::visit(overloaded{
				  [&](const Rendering::DrawStaticMeshTriangle& StaticMesh) {
					if (m_OnMeshDrawTriangle)
						m_OnMeshDrawTriangle(StaticMesh, _Cmd);
				  },
				  [&](const Rendering::DrawStaticMeshMeshlet& StaticMesh) {
					if (m_OnMeshDrawMeshlet)
						m_OnMeshDrawMeshlet(StaticMesh, _Cmd);
				  },
				}, DrawItem.Data);
		}
	}
}

