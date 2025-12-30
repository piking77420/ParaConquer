#include "Rendering/Renderer.hpp"

#include <Thread>
#include <PerfRegion.hpp>

#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/Vertex.hpp"
#include "Resources/ResourceManager.hpp"
#include "Time/CoreTime.hpp"
#include "Math/MatrixTransformation.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "Rendering/RenderPasses/RenderPass.hpp"


#include "Math/ToolboxTypedef.hpp"
#include "Rendering/RenderSystem.hpp"
#include "Resources/ShaderSourceBinary.hpp"

#include "VulkanCommandList.hpp"
#include "Resources/ComputeShader.hpp"
#include "Rendering/Sampler.hpp"

using namespace PC_CORE::Rendering;


Renderer::~Renderer()
{
    
}

void Renderer::Init(Rhi& _Rhi)
{
    return;
}

PC_CORE_API void PC_CORE::Rendering::Renderer::Update(const RenderView& _view, const RenderGraph& RenderGraph)
{
    
}

PC_CORE_API void PC_CORE::Rendering::Renderer::Excute(const RenderView& _view, const RenderGraph& RenderGraph)
{
   
}
