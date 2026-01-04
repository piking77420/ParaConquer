#include "Rendering/Renderer.hpp"

#include <PerfRegion.hpp>
#include <Thread>

#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Rendering/RenderPasses/ForwardPass.hpp"
#include "Rendering/RenderPasses/ToneMapPass.hpp"
#include "Rendering/RenderView.hpp"
#include "Resources/StaticMesh.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"


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

       m_RenderGraph.Clear();
       m_RenderGraph.AddRenderPass<Pass::FowardPass>();
       m_RenderGraph.AddRenderPass<Pass::ToneMapPass>();

       InitRhiRenderPasses(_View);
       InitShaders(_View);

       RendererPassBuildContext buildContext(*m_CommandList, m_Rhi, _View, *this, m_RenderGraph);
       m_RenderGraph.Build(buildContext);

   }

   void Renderer::Excute(RenderView& _View, const RenderingWorldData& RenderingWorldData)
   {
       RendererPassExecuteContext executeContext(*m_CommandList, m_Rhi, _View, *this, m_RenderGraph, RenderingWorldData);

       m_RenderGraph.Execute(executeContext, _View);
   }

   void Renderer::InitRhiRenderPasses(const RenderView& _View)
   {
       {
           forwardPass.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& ColorAttachement = forwardPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::FragmentShaderResource);

           // Set Depth
           const RenderPassAttachementDescriptor& DepthAttachement = forwardPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S01)
               .SetRhiFormat(RhiFormat::D24UnormS8Uint)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::DepthStencilWrite)
               .SetFinalImageState(RhiResourceState::DepthStencilWrite);

           // SubPass 0
           forwardPass
               ->CreateSubPass()
               .SetType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementRef(AttachementRef(ColorAttachement, RhiResourceState::RenderTarget))
               .SetDepthAttachementRef(AttachementRef(DepthAttachement, RhiResourceState::DepthStencilWrite));

           forwardPass
               ->SetName("ForwardPass")
               .Build();
       }

       {
           toneMapPass.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = toneMapPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::FragmentShaderResource);

           toneMapPass
               ->CreateSubPass()
               .SetType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));

           toneMapPass
               ->SetName("ToneMap temp")
               .Build();
       }
      
   }

   void Renderer::InitShaders([[maybe_unsed]] const RenderView& _View)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Vertex, ResourceManager::Get<ShaderSourceBinary>("DrawQuadTriangle.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Pixel, ResourceManager::Get<ShaderSourceBinary>("SampleSingleTexture.ps.hlsl.binary")->GetCode() }
           };

           drawTextureQuad.reset(m_Rhi.CreateRhiShaderProgram());
           drawTextureQuad
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*toneMapPass)
               .SetName("DrawQuadTriangle")
               .Build();
       }

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Vertex, ResourceManager::Get<ShaderSourceBinary>("Forward.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Pixel, ResourceManager::Get<ShaderSourceBinary>("Forward.ps.hlsl.binary")->GetCode() }
           };

           fowardShader.reset(m_Rhi.CreateRhiShaderProgram());
           fowardShader
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*forwardPass)
               .SetDepthTest(true)
               .SetDepthWrite(true)
               .SetVertexAttributeDescriptions(StaticMeshVertex::GetAttributeDescriptions(0))
               .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) })
               .SetName("FowardShader")
               .Build();
       }
   }

}

