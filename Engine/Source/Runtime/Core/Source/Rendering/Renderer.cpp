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
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);


       m_CommandList.reset(m_Rhi.CreateCommandList());
       m_CommandList
           ->SetName("RendererCommandList")
           .Build();

       linearClampToEdgeSampler.reset(m_Rhi.CreateSampler());
       linearClampToEdgeSampler
           ->SetMagFilter(Filter::Linear)
           .SetMinFilter(Filter::Linear)
           .SetU(SamplerAddressMode::ClampToEdge)
           .SetV(SamplerAddressMode::ClampToEdge)
           .SetW(SamplerAddressMode::ClampToEdge)
           .SetName("Linear Clamp To EdgeSampler")
           .Build();


       InitShaders(_View);

       m_RenderGraph.Clear();
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

   void Renderer::InitShaders([[maybe_unsed]] const RenderView& _View)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageType::Vertex, ResourceManager::Get<ShaderSourceBinary>("DrawQuadTriangle.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageType::Pixel, ResourceManager::Get<ShaderSourceBinary>("SampleSingleTexture.ps.hlsl.binary")->GetCode() }
           };

           drawTextureQuad.reset(m_Rhi.CreateRhiShaderProgram());
           drawTextureQuad
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetName("DrawQuadTriangle")
               .Build();
       }

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageType::Vertex, ResourceManager::Get<ShaderSourceBinary>("Forward.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageType::Pixel, ResourceManager::Get<ShaderSourceBinary>("Forward.ps.hlsl.binary")->GetCode() }
           };

           fowardShader.reset(m_Rhi.CreateRhiShaderProgram());
           fowardShader
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(2)
               .SetShaderModules(shaderModules)
               .SetName("FowardShader")
               .Build();
       }
   }

}

