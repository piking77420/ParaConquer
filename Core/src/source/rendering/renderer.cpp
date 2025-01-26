#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
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
   
   ShaderGraphicPointInfo shaderGraphicPointInfo =
      {
      .rasterizerInfo = rasterizerInfo,
      .vertexInputBindingDescritions = {},
      .vertexAttributeDescriptions = {}
      };

   std::vector<std::pair<ShaderStageType, std::string>> source =
   {
      {
         ShaderStageType::VERTEX,
         "main_spv.vert"
      },
      {
         ShaderStageType::VERTEX,
         "main_spv.frag"
      }
   };
   
   const ShaderInfo shaderInfo =
      {
      .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
      .shaderInfoData = shaderGraphicPointInfo,
      .shaderSources = source
      };
   
   PC_CORE::ProgramShaderCreateInfo triangleCreateInfo =
      {
      .prograShaderName = "Main",
      .shaderInfo = shaderInfo
      };

   
   m_Main = PC_CORE::Rhi::CreateShader(triangleCreateInfo);
}
