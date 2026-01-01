#include "Rendering/Renderer.hpp"

#include <PerfRegion.hpp>
#include <Thread>

#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Rendering/RenderView.hpp"



namespace PC_CORE::Rendering
{
    Renderer::Renderer(Rhi& _Rhi)
        : m_Rhi(_Rhi)
        , m_RenderGraph(m_Rhi)
    {
       
    }

   void Renderer::Build(const RenderView& _View)
   {
       m_CommandList.reset(m_Rhi.CreateCommandList());
       m_CommandList
           ->SetName("RendererCommandList")
           .Build();


       m_RenderGraph.AddRenderPass(&m_FowardPass);
       m_RenderGraph.AddRenderPass(&m_ToneMapPass);

       RendererPassBuildContext buildContext(*m_CommandList, m_Rhi, _View, *this, m_RenderGraph);
       m_RenderGraph.Build(buildContext);
   }

   void Renderer::Excute(const RenderView& _View)
   {
       RenderingWorldData RenderingWorldData;
       RendererPassExecuteContext executeContext(*m_CommandList, m_Rhi, _View, *this, m_RenderGraph, RenderingWorldData);

       m_RenderGraph.Execute(executeContext);
   }

   void Renderer::InitShaders()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageType::Vertex, ResourceManager::Get<ShaderSourceBinary>("DrawQuadTriangle.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageType::Pixel, ResourceManager::Get<ShaderSourceBinary>("DrawQuadTriangle.ps.hlsl.binary")->GetCode() }
           };

           drawTextureQuad.reset(m_Rhi.CreateRhiShaderProgram());
           drawTextureQuad
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetName("DrawQuadTriangle")
               .Build();
       }
   }

}

