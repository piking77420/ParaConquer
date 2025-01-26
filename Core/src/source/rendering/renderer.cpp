#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void Renderer::Init()
{
   constexpr RasterizerInfo rasterizerInfo =
   {
   .polygonMode = PolygonMode::Fill,
   .cullModeFlag = CullModeFlagBit::None,
   .frontFace = FrontFace::CounterClockwise
   };
   

   const ShaderGraphicPointInfo shaderGraphicPointInfo =
      {
      .rasterizerInfo = rasterizerInfo,
      .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
      .vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0)
      };

   const std::vector<std::pair<ShaderStageType, std::string>> source =
   {
      {
         ShaderStageType::VERTEX,
         "main_spv.vert"
      },
      {
         ShaderStageType::FRAGMENT,
         "main_spv.frag"
      }
   };
   
   const ShaderInfo shaderInfo =
      {
      .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
      .shaderInfoData = shaderGraphicPointInfo,
      .shaderSources = source
      };
   
   const PC_CORE::ProgramShaderCreateInfo triangleCreateInfo =
      {
      .prograShaderName = "Main",
      .shaderInfo = shaderInfo,
      .renderPass = RhiContext::GetContext().swapChain->GetSwapChainRenderPass(),
      };

   
   m_Main = PC_CORE::Rhi::CreateShader(triangleCreateInfo);
}
