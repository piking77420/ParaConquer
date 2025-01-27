#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void Renderer::Init()
{
    m_RhiContext = Rhi::GetRhiContext();
   
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

   constexpr  CommandListCreateInfo commandListCreateInfo =
      {
      ._commandPoolFamily = CommandPoolFamily::Graphics
      };
   
   m_CommandList = PC_CORE::Rhi::CreateCommandList(commandListCreateInfo);   
}

bool Renderer::BeginDraw(Window* _window)
{
   m_RhiContext->swapChain->WaitForFrame();
   bool hasGetImage = m_RhiContext->swapChain->GetSwapChainImageIndex(_window);
   if (!hasGetImage)
   {
      return hasGetImage;
   }

   m_CommandList->Reset();
   m_CommandList->BeginRecordCommands();
}

void Renderer::Render()
{
   uint32_t swapChainWidht = m_RhiContext->swapChain->GetWidth();
   uint32_t swapChainHeight = m_RhiContext->swapChain->GetHeight();

   const BeginRenderPassInfo BeginRenderPassInfo =
      {
      .renderPass = RhiContext::GetContext().swapChain->GetSwapChainRenderPass(),
      .frameBuffer =  RhiContext::GetContext().swapChain->GetFrameBuffer(),
      .renderOffSet = {0,0},
      .extent = {swapChainWidht,swapChainHeight},
      .clearColor = {0,0,0,1.f}
      };
   m_CommandList->BeginRenderPass(BeginRenderPassInfo);
   m_CommandList->BindProgram(m_Main);

   const ViewportInfo viewportInfo =
      {
      .transform = {0,0},
      .size = {static_cast<float>(swapChainWidht),static_cast<float>(swapChainHeight)},
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
      .scissorsOff = {0,0},
      .scissorsextent = {swapChainWidht, swapChainHeight} 
      };
   m_CommandList->SetViewPort(viewportInfo);

   m_CommandList->Draw(3, 1, 0, 0);
   m_CommandList->EndRenderPass();
   m_CommandList->EndRecordCommands();
   
}


void Renderer::SwapBuffers(Window* _window)
{
    m_RhiContext->swapChain->Present(m_CommandList.get(), _window);
    Rhi::NextFrame();
}

