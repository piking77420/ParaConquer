#include "Rendering/Renderer.hpp"

#include <PerfRegion.hpp>
#include <Thread>

#include "Color.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Rendering/RenderPasses/ForwardPass.hpp"
#include "Rendering/RenderPasses/ToneMapPass.hpp"
#include "Rendering/RenderView.hpp"
#include <Rendering/PipelineCache.hpp>
#include "Resources/StaticMesh.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"


namespace PC_CORE::Rendering
{
    Renderer::Renderer(Rhi& _Rhi, PC_CORE::Window& Window)
        : m_Rhi(_Rhi)
        , m_Window(Window)
        , m_RenderGraph(m_Rhi)
    {
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

        InitRhiRenderPasses();

        if (!InstanceBuffer)
        {
            InstanceBuffer.reset(m_Rhi.CreateBuffer());
            InstanceBuffer
                ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
                .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::PerFrame)
                .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
                .SetSizeInBytes(sizeof(Gpu::RenderInstance) * 4096 * 2)
                .SetName("Instances Buffer")
                .Build();
        }

    }

    uint64_t ComputeSortKey(bool _IsOpaque,
        const uint64_t& _DepthKey, 
        const uint64_t& _MaterialKey, 
        const uint64_t& _PipelineKey)
    {

        uint64_t Key = 0;

        if (_IsOpaque)
        {
            Key =
                ((_PipelineKey & 0xFFFF) << 48) |
                ((_MaterialKey & 0xFFFF) << 32) |
                _DepthKey; // depth is lsb
        }
        else
        {
            // Reverse depth so that larger values correspond to closer objects.
            // When sorting ascending, this results in back-to-front ordering.
            Key =
                ((std::numeric_limits<uint64_t>::max() - _DepthKey) << 32) |
                (_MaterialKey << 16) |
                static_cast<uint64_t>(_PipelineKey);
        }

        return Key;
    }

    void Renderer::RenderFrame()
    {
        m_CommandList->BeginRecordCommands();
        if (OnRender)
            OnRender(*m_CommandList);

        auto& swapChain = m_Rhi.GetRhiContext().rhiSwapChain;
        constexpr std::array<float, 4> Color = {0.5f, 0.5f, 0.5f, 0.5f,};

        m_CommandList->BeginDebugLabel("SwapChain", Color);
        swapChain->BeginSwapChainRenderPass(m_CommandList.get());
        if (OnSwapchainPass)
            OnSwapchainPass(*m_CommandList);
        swapChain->EndSwapChainRenderPass(m_CommandList.get());
        m_CommandList->EndDebugLabel();

        m_CommandList->EndRecordCommands();
        m_Rhi.GetRhiContext().SendEnqueuCommand(m_CommandList.get(), GpuPipelineStage::ColorAttachmentOutput);
    }

   void Renderer::Build(const RenderView& _View, const std::function<void(RenderGraph&)>& InitRenderGraphFunction)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);
       m_RenderGraph.Clear();
       InitRenderGraphFunction(m_RenderGraph);
       InitShaders(); // should be call in constructor however shader creation are build after app cronstructor
       InitDebugResource();

       RendererPassBuildContext buildContext(m_Rhi, m_RenderGraph, _View, *this);
       m_RenderGraph.Build(buildContext);
   }

   void Renderer::Excute(RenderView& _View, PipelineCache& PipelineCache, const RenderingWorldData& RenderingWorldData)
   {
       PrepareInstanceBuffer(RenderingWorldData);
       UpdateEnvironement(_View, RenderingWorldData);
       BuildDrawLists(_View, RenderingWorldData);
       UploadRenderInstanceID();


       RendererPassExecuteContext executeContext(*m_CommandList, m_Rhi, m_RenderGraph, PipelineCache, _View, *this, RenderingWorldData);
       PC_CORE::Rendering::PipelineCacheHandleID id{};
       for (size_t i = 0; i < 2; i++)       
       {
           PipelineCache::ModuleEntryList list;
           list.Reserve(2);
           auto& vertexShader = list.Next();
           vertexShader.ShaderSourcePath = "/Shaders/DrawQuadTriangle.vs.hlsl";

           auto& fragmentShader = list.Next();
           fragmentShader.ShaderSourcePath = "/Shaders/SampleSingleTexture.ps.hlsl";
           RhiGraphicPipeline::Descriptor descriptor;
           descriptor
               .SetRenderPass(*colorLinearPass);

           PipelineCache::PipelineQueryResult Result = executeContext.PipelineCache.CreateOrGetGraphicPipelineCache(&id, "DrawQuadTriangle", list, descriptor);
           assert(Result);
           RhiPipeline& Pipeline = Result->get();
       }

       PC_CORE::Rendering::PipelineCacheHandleID idCompute{};
       for (size_t i = 0; i < 2; i++)
       {
           PipelineCache::ModuleEntryList list;
           auto& compute = list.Next();
           compute.ShaderSourcePath = "/Shaders/PostProcess/ToneMapping/Aces.cs.hlsl";

           PipelineCache::PipelineQueryResult Result = executeContext.PipelineCache.CreateOrGetComputePipelineCache(&idCompute, "Aces", list);
           assert(Result);
           RhiPipeline& Pipeline = Result->get();
       }
        
       m_RenderGraph.Execute(executeContext, _View);     
   }

   void Renderer::SetPipelineCache(PC_CORE::Rendering::PipelineCache* _PipelineCache)
   {
       assert(_PipelineCache);
       PipelinesCache = _PipelineCache;
   }

   void Renderer::InitRhiRenderPasses()
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
               .SetFinalImageState(RhiResourceState::PixelShaderResource);

           // Set Depth
           const RenderPassAttachementDescriptor& DepthAttachement = forwardPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S01)
               .SetRhiFormat(RhiFormat::D24UnormS8Uint)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::DepthStencilWrite);

           // SubPass 0
           forwardPass
               ->CreateSubPass()
               .SetAttachementRef(AttachementRef(ColorAttachement, RhiResourceState::RenderTarget))
               .SetDepthAttachementRef(AttachementRef(DepthAttachement, RhiResourceState::DepthStencilWrite));

           forwardPass
               ->SetName("ForwardPass")
               .Build();
       }

       {
           colorLinearPass.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = colorLinearPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::PixelShaderResource);

           colorLinearPass
               ->CreateSubPass()
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));

           colorLinearPass
               ->SetName("ColorLinearPass")
               .Build();
       }

       {
           colorHDRPass.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = colorHDRPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R32G32B32A32Sfloat)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::PixelShaderResource);

           colorHDRPass
               ->CreateSubPass()
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));

           colorHDRPass
               ->SetName("colorHDRPass")
               .Build();
       }

       {
           LinearClearColorClearStoreDepth.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = LinearClearColorClearStoreDepth
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::PixelShaderResource);

           const RenderPassAttachementDescriptor& DepthAttachement = LinearClearColorClearStoreDepth
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S01)
               .SetRhiFormat(RhiFormat::D24UnormS8Uint)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::DepthStencilWrite);

           LinearClearColorClearStoreDepth
               ->CreateSubPass()
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget))
               .SetDepthAttachementRef(AttachementRef(DepthAttachement, RhiResourceState::DepthStencilWrite));

           LinearClearColorClearStoreDepth
               ->SetName("LinearClearColorClearStoreDepth")
               .Build();
       }

       {
           LoadHdrColorLoadStoreDepth.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = LoadHdrColorLoadStoreDepth
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R16G16B16A16Sfloat)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Load)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::RenderTarget)
               .SetFinalImageState(RhiResourceState::PixelShaderResource);

           const RenderPassAttachementDescriptor& DepthAttachement = LoadHdrColorLoadStoreDepth
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S01)
               .SetRhiFormat(RhiFormat::D24UnormS8Uint)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Load)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::DepthStencilWrite)
               .SetFinalImageState(RhiResourceState::DepthStencilRead);

           LoadHdrColorLoadStoreDepth
               ->CreateSubPass()
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget))
               .SetDepthAttachementRef(AttachementRef(DepthAttachement, RhiResourceState::DepthStencilWrite));

           LoadHdrColorLoadStoreDepth
               ->SetName("LoadHdrColorLoadStoreDepth")
               .Build();
       }

       {
           BRDFLutPass.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = BRDFLutPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R16G16Sfloat)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::DontCare)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::PixelShaderResource);

           BRDFLutPass
               ->CreateSubPass()
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));

           BRDFLutPass
               ->SetName("BRDF LUT Pass")
               .Build();
       }
      
   }

   void Renderer::InitShaders()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);
       /*
       {
           const std::vector<const ShaderSourceBinary*> shaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("Aces.cs.hlsl.binary").Get() },
           };
           toneMapAces.reset(m_Rhi.CreateRhiComputePipeline());
           toneMapAces
               ->SetShaderModules(shaderModules)
               .SetName("ToneMapAces")
               .Build();
       }

       

       {
           const std::vector<const ShaderSourceBinary*> shaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("DrawQuadTriangle.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("SampleSingleTexture.ps.hlsl.binary").Get() }
           };

           drawTextureQuad.reset(m_Rhi.CreateRhiGraphicPipeline());
           drawTextureQuad
               ->SetRenderPass(*colorLinearPass)
               .SetName("DrawQuadTriangle")
               .SetShaderModules(shaderModules)
               .Build();
       }

       auto InitShaderProgramForwardPass = [&]<bool IsTransparent>(
           RhiRenderPass& RhiRenderPass,
           std::unique_ptr<RhiGraphicPipeline>&Shader,
           const std::vector <const ShaderSourceBinary*> &ShaderModules,
           std::string ShaderName)
           {
               Shader.reset(m_Rhi.CreateRhiGraphicPipeline());
               Shader
                   ->SetRenderPass(RhiRenderPass)
                   .SetDepthTest(true)
                   .SetDepthWrite(true)
                   .SetVertexAttributeDescriptions(StaticMeshVertex::GetAttributeDescriptions(0))
                   .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) })
                   .SetShaderModules(ShaderModules)
                   .SetName(ShaderName);
                    
               if constexpr (IsTransparent)
               {
                   constexpr PC_CORE::RhiGraphicPipeline::BlendState blenstate =
                   {
                       .ColorSrcFactor = PC_CORE::BlendFactor::SrcAlpha,
                       .ColorDstFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
                       .ColorOp = PC_CORE::BlendOp::Add,

                       .AlphaSrcFactor = PC_CORE::BlendFactor::One,
                       .AlphaDstFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
                       .AlphaOp = PC_CORE::BlendOp::Add,

                       .BlendMask = PC_CORE::ColorComponent::ColorComponentRGBA
                   };
                   Shader->SetBlendState(blenstate)
                   .Build();
                    
               }
               else
               {
                   Shader->SetCullMode(RhiGraphicPipeline::CullModeFlagBits::CullBack);
                   Shader->Build();
               }
       };

       {
           // Foward
           const std::vector<const ShaderSourceBinary*> TriangleModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("Forward.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("ForwardLit.ps.hlsl.binary").Get()},
           };

           InitShaderProgramForwardPass.template operator() < false > (*forwardPass, opaqueFowardShader, TriangleModules, "Opaque FowardShader");
           InitShaderProgramForwardPass.template operator() < true > (*forwardPass, transparentForwardShader, TriangleModules, "Transparent FowardShader");
       }
       
       {
           // Forward but with meshsahder
           const std::vector<const ShaderSourceBinary*> MeshetsModulesModules
           {
              { ResourceManager::Get<ShaderSourceBinary>("DrawMeshlet.as.hlsl.binary").Get()},
              { ResourceManager::Get<ShaderSourceBinary>("ForwardMeshlet.ms.hlsl.binary").Get()},
              { ResourceManager::Get<ShaderSourceBinary>("ForwardLit.ps.hlsl.binary").Get()}
           };

           InitShaderProgramForwardPass.template operator() < false > (*forwardPass, opaqueFowardShaderMeshlet, MeshetsModulesModules, "Opaque FowardShader Meshlet");
           InitShaderProgramForwardPass.template operator() < true > (*forwardPass, transparentForwardShaderMeshlet, MeshetsModulesModules, "Transparent FowardShader Meshlet");
       }
       
       {
           const std::vector<const ShaderSourceBinary*> shaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangle.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangle.ps.hlsl.binary").Get()},
           };
           InitShaderProgramForwardPass.template operator() < false > (*LinearClearColorClearStoreDepth, DrawTriangle, shaderModules, "DrawMeshTriangle");

       }

       {
               // DrawTriangle MeshShader
               const std::vector<const ShaderSourceBinary*> shaderModules
               {
                   { ResourceManager::Get<ShaderSourceBinary>("DrawMeshlet.as.hlsl.binary").Get()},
                   { ResourceManager::Get<ShaderSourceBinary>("DrawTriangleMeshlet.ms.hlsl.binary").Get()},
                   { ResourceManager::Get<ShaderSourceBinary>("DrawTriangleMeshlet.ps.hlsl.binary").Get()}
               };

               InitShaderProgramForwardPass.template operator() < false > (*LinearClearColorClearStoreDepth, DrawMeshTriangleMeshlet, shaderModules, "DrawMeshTriangleMeshlet");
           }

           {

           const std::vector<const ShaderSourceBinary*> shaderModules
           {
               {  ResourceManager::Get<ShaderSourceBinary>("DrawMeshlet.as.hlsl.binary").Get()},
               {  ResourceManager::Get<ShaderSourceBinary>("DrawMeshletColor.ms.hlsl.binary").Get()},
               {  ResourceManager::Get<ShaderSourceBinary>("DrawMeshletColor.ps.hlsl.binary").Get()}
           };

           InitShaderProgramForwardPass.template operator() < false > (*LinearClearColorClearStoreDepth, DrawMeshletColor, shaderModules, "DrawMeshMeshelet");
       }

       {
           auto DebugDrawShader = [&](std::unique_ptr<RhiGraphicPipeline>& Program, 
               const std::vector<const ShaderSourceBinary*>& ShaderModules,
               std::string ShaderName,
               bool UseVertexBinding) {
               Program.reset(m_Rhi.CreateRhiGraphicPipeline());
               Program
                   ->SetRenderPass(*LoadHdrColorLoadStoreDepth)
                   .SetDepthTest(true)
                   .SetDepthWrite(true)
                   .SetShaderModules(ShaderModules);

               if (UseVertexBinding)
               {
                   Program->SetVertexAttributeDescriptions({ VertexAttributeDescription{
                    .Binding = 0,
                    .Location = 0,
                    .Format = RhiFormat::R32G32B32A32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Position)
                    } })
                       .SetVertexInputBindingDescritions({
                                                        {
                                                        .Binding = 0,
                                                        .Stride = sizeof(Tbx::Vector4f),
                                                        .VertexInputRate = VertexInputRate::Vertex
                                                        } });
               }


               Program
                   ->SetName(ShaderName)
                   .Build();
               };


           {
               const std::vector<const ShaderSourceBinary*> shaderModules
               {
                   { ResourceManager::Get<ShaderSourceBinary>("DebugDrawInstanced.vs.hlsl.binary").Get()},
                   { ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary").Get()}
               };

               DebugDrawShader(DrawDebugShapeInstanced, shaderModules, "DebugInstanceDraw", true);
           }

           {
               const std::vector<const ShaderSourceBinary*> shaderModules
               {
                   { ResourceManager::Get<ShaderSourceBinary>("DebugDrawFrustum.vs.hlsl.binary").Get()},
                   { ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary").Get()}
               };

               DebugDrawShader(DrawDebugShapeFrustum, shaderModules, "DebugDrawFrustum", false);
           }

           {
               const std::vector<const ShaderSourceBinary*> shaderModules
               {
                   { ResourceManager::Get<ShaderSourceBinary>("DrawMeshletBound.as.hlsl.binary").Get()},
                   { ResourceManager::Get<ShaderSourceBinary>("DrawMeshletBound.ms.hlsl.binary").Get()},
                   { ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary").Get()}
               };

               DebugDrawShader(DrawDebugMeshletBound, shaderModules, "DrawDebugMeshletBound", false);
           }
           }

       {
           const std::vector<const ShaderSourceBinary*> ShaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("CubeMap.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("EquirectangularToCubeMap.ps.hlsl.binary").Get()}
           };

           EquilateralToSkyBox.reset(m_Rhi.CreateRhiGraphicPipeline());
           EquilateralToSkyBox
               ->SetRenderPass(*colorHDRPass)
               .SetDepthTest(false)
               .SetDepthWrite(false)
               .SetName("EquilateralToSkyBox")
               .SetShaderModules(ShaderModules)
               .Build();
       }

       {
           // Skybox
           const std::vector<const ShaderSourceBinary*> ShaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("Skybox.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("Skybox.ps.hlsl.binary").Get()}
           };

           DrawSkyBoxPipeline.reset(m_Rhi.CreateRhiGraphicPipeline());
           DrawSkyBoxPipeline
               ->SetRenderPass(*forwardPass)
               .SetDepthTest(true)
               .SetDepthCompareOp(CompareOp::LessOrEqual)
               .SetDepthWrite(false)
               .SetName("Skybox")
               .SetShaderModules(ShaderModules)
               .Build();
       }

       {
           // IrradianceConvolution
           const std::vector<const ShaderSourceBinary*> ShaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("CubeMap.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("IrradianceConvolution.ps.hlsl.binary").Get()}
           };

           IrradianceConvolution.reset(m_Rhi.CreateRhiGraphicPipeline());
           IrradianceConvolution
               ->SetRenderPass(*colorHDRPass)
               .SetDepthTest(false)
               .SetDepthWrite(false)
               .SetName("IrradianceConvolution")
               .SetShaderModules(ShaderModules)
               .Build();
       }

       {
           // PrefilterEnvironement
           const std::vector<const ShaderSourceBinary*> ShaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("CubeMap.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("PrefilterEnvironement.ps.hlsl.binary").Get()}
           };

           PrefilterEnvironement.reset(m_Rhi.CreateRhiGraphicPipeline());
           PrefilterEnvironement
               ->SetRenderPass(*colorHDRPass)
               .SetDepthTest(false)
               .SetDepthWrite(false)
               .SetName("PrefilterEnvironement")
               .SetShaderModules(ShaderModules)
               .Build();
       }

       // BRDFLutPipeline
       {
           const std::vector<const ShaderSourceBinary*> ShaderModules
           {
               { ResourceManager::Get<ShaderSourceBinary>("DrawQuadTriangle.vs.hlsl.binary").Get()},
               { ResourceManager::Get<ShaderSourceBinary>("BRDFLUT.ps.hlsl.binary").Get()}
           };

           BRDFLutPipeline.reset(m_Rhi.CreateRhiGraphicPipeline());
           BRDFLutPipeline
               ->SetRenderPass(*BRDFLutPass)
               .SetDepthTest(false)
               .SetDepthWrite(false)
               .SetName("BRDFLutPipeline")
               .SetShaderModules(ShaderModules)
               .Build();
       }
       */
   }

   void Renderer::UploadRenderInstanceID()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering)
       DebugLabelScope _(*m_CommandList, "Upload RenderInstanceID", { 1.0f,0.2f, 0.f,1.0 });

       PC_CORE::BufferStateTransition Transfert{};
       Transfert.Buffer = InstanceBuffer.get();
       Transfert.Offset = 0u;
       Transfert.Size = PC_CORE::WHOLE_SIZE;
       // Uppload Instance Buffer
       InstanceBuffer->UploadData(m_CommandList.get(), m_InstanceBufferCpu.data(), m_InstanceBufferCpu.size() * sizeof(m_InstanceBufferCpu[0]));
       m_CommandList->Barrier(RhiResourceState::CopyDst, RhiResourceState::VertexShaderResource, {}, std::span(&Transfert, 1));
   }

   void Renderer::UpdateEnvironement(RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       if (!RenderingWorldData.Environement || !RenderingWorldData.Environement->isDirty)
           return;

       if (!SkyBoxDescriptorSet)
       {
           SkyBoxDescriptorSet.reset(m_Rhi.CreateDescriptorSet());
           SkyBoxDescriptorSet
               ->BindTexture(RhiShaderStageBits::Pixel, 0, RenderingWorldData.Environement->SkyBox, linearClampToEdgeSampler.get())
               .SetName("SkyBoxDescriptorSet")
               .Build();
       }

       if (!EnvironementDescriptorSet)
       {
           EnvironementDescriptorSet.reset(m_Rhi.CreateDescriptorSet());
           EnvironementDescriptorSet
               ->BindTexture(RhiShaderStageBits::Pixel, 0, RenderingWorldData.Environement->IrradianceMap, linearClampToEdgeSampler.get())
               .BindTexture(RhiShaderStageBits::Pixel, 1, RenderingWorldData.Environement->PrefilterMap, linearClampToEdgeSampler.get())
               .BindTexture(RhiShaderStageBits::Pixel, 2, RenderingWorldData.Environement->BRDF, linearClampToEdgeSampler.get())
               .SetName("Environemement DescriptorSet")
               .Build();
       }

   }

   void Renderer::BuildDrawLists(RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       OpaqueList.Clear();
       TransparentList.Clear();
       DebugDrawList.Clear();
       Skybox.Clear();


       FillListStaticMesh(_view, RenderingWorldData);
       FillListDebugDraw(_view, RenderingWorldData);
       FillSkyBox(_view, RenderingWorldData);
       SortList();
   }

   void Renderer::FillListStaticMesh(RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);
       for (const auto& StaticMeshComponentData : RenderingWorldData.StaticMeshComponentData)
       {
           const StaticMesh* StaticMesh = StaticMeshComponentData.StaticMesh;
           const StaticMeshData& StaticMeshData = StaticMesh->GetStaticMeshData();

           const MotionCore::Aabb<double> MeshAABBW = StaticMeshComponentData.StaticMesh->GetAabb().GetTransformed(StaticMeshComponentData.WorldMatrix);
           const auto MeshAABBCenter = MeshAABBW.GetCenter();
           const auto MeshAABBExtend = MeshAABBW.GetExtend();
           const bool MeshIsOnFrustum = _view.FrustumWorld.IsOnFrustum(MeshAABBCenter, MeshAABBExtend);

           if (!MeshIsOnFrustum)
             continue;

           // Pick Lod
           uint32_t LODIndex = 0;
           const double DistanceAABBToCamera = (MeshAABBExtend - _view.ViewPosition).Magnitude();
           const double BoundingSphereRadius = (MeshAABBExtend).Magnitude();

           const Tbx::Matrix4x4d& Model = StaticMeshComponentData.WorldMatrix;
  
           if (!StaticMesh->GetLodThreshold().empty())
           {
               LODIndex = PickLodCount(StaticMesh->GetLodThreshold(), BoundingSphereRadius, DistanceAABBToCamera, _view.Fov);
           }

           const auto& Lod = StaticMeshData.MeshLods[LODIndex];
           const std::vector<MeshDrawCommand>& DrawCommands = Lod.DrawCommands;
           for (const auto& Dcmd : DrawCommands)
           {
               const MeshSection& MeshSection = Lod.MeshesSections[Dcmd.MeshSectionIndex];
               const Material* Material = StaticMeshComponentData.Materials.at(MeshSection.MaterialIndex);
               if (!Material)
                   continue; // to do get dummy mat

               const MotionCore::Aabb<double> MeshSectionAABBW = Dcmd.GlobalModelAABB.GetTransformed(StaticMeshComponentData.WorldMatrix);
               const auto MeshSectionAABBCenter = MeshSectionAABBW.GetCenter();
               const auto MeshSectionAABBExtend = MeshSectionAABBW.GetExtend();
               if (!_view.FrustumWorld.IsOnFrustum(MeshSectionAABBCenter, MeshSectionAABBExtend))
                 continue;

               const bool isOpaque = Material->GetMaterialType() == MaterialType::Opaque;
               DrawList& DrawList = isOpaque ? OpaqueList : TransparentList;
               DrawItem& item = DrawList.EmplaceBack();

               // Compute Gpu Matrix
               const Tbx::Matrix4x4d GlobleMatrixModel = Model * Dcmd.GlobalModelMatrix;
               // Position
               const Tbx::Matrix4x4f Model = Tbx::Matrix4x4f(GlobleMatrixModel);

               // Normal
               const Tbx::Matrix3x3d ModelD3 = Tbx::ToMatrix3x3(GlobleMatrixModel);
               const Tbx::Matrix4x4f NormalInverMatrix = static_cast<Tbx::Matrix4x4f>(Tbx::ToMatrix4x4(ModelD3.Invert().Transpose()));

               // Instance Matrix Update
               item.InstanceIndex = m_InstanceBufferCpu.size();
               // Copy Data to gpu
               auto& RenderInstance = m_InstanceBufferCpu.emplace_back();
               std::memcpy(RenderInstance.Model.data.data(), Model.data, sizeof(RenderInstance.Model));
               std::memcpy(RenderInstance.NormalInvertMatrix.data.data(), NormalInverMatrix.data, sizeof(RenderInstance.NormalInvertMatrix));

               switch (m_RenderGraph.GetRenderMode())
               {
               case RenderMode::TriangleBased:
               {
                   DrawStaticMeshTriangle& Descritptor = item.Data.emplace<DrawStaticMeshTriangle>();

                   Descritptor.ShaderProgram = isOpaque ? opaqueFowardShader.get() : transparentForwardShader.get();
                   Descritptor.MaterialDescriptor = Material->GetDescriptorSet();
                   Descritptor.VertexBuffer = StaticMesh->GetVertexBuffer(LODIndex).Get();
                   Descritptor.IndexBuffer = StaticMesh->GetIndexBuffer(LODIndex).Get();

                   Descritptor.MaterialDescriptorOffset = Material->GetMaterialStride() * m_Rhi.GetFrameIndex();
                   Descritptor.VertexOffset = MeshSection.MeshDataDescriptor.VertexOffset;
                   Descritptor.IndexOffset = MeshSection.MeshDataDescriptor.IndicesOffset;
                   Descritptor.IndexCount = MeshSection.MeshDataDescriptor.IndicesCount;
                   Descritptor.IndexFormat = StaticMesh->GetIndexBuffer(LODIndex).GetIndexFormat();

                   const uint64_t shaderKey = reinterpret_cast<uint64_t>(Descritptor.ShaderProgram) >> 4;
                   const uint64_t materialKey = reinterpret_cast<uint64_t>(Descritptor.MaterialDescriptor) >> 4;
                   const uint64_t depthKey = static_cast<uint64_t>(DistanceAABBToCamera * FIXED_POINT_NUMBER);

                   item.SortKey = ComputeSortKey(isOpaque, depthKey, materialKey, shaderKey);
               }
                   break;
               case RenderMode::ClusterBased:
               {
                   DrawStaticMeshMeshlet& Descritptor = item.Data.emplace<DrawStaticMeshMeshlet>();

                   Descritptor.ShaderProgram = isOpaque ? opaqueFowardShaderMeshlet.get() : transparentForwardShaderMeshlet.get();
                   Descritptor.MaterialDescriptor = Material->GetDescriptorSet();
                   Descritptor.MeshletDescriptor = StaticMesh->GetMeshletDescriptor(LODIndex);
                   Descritptor.MeshletBoundDescriptor = StaticMesh->GetMeshletBoundsDescriptor(LODIndex);

                   Descritptor.MaterialDescriptorOffset = Material->GetMaterialStride() * m_Rhi.GetFrameIndex();
                   Descritptor.VertexOffset = MeshSection.MeshDataDescriptor.VertexOffset;
                   Descritptor.MeshletOffset = MeshSection.MeshDataDescriptor.MeshetOffset;
                   Descritptor.MeshletCount = MeshSection.MeshDataDescriptor.MeshetCount;
                   Descritptor.SubMeshTriangleVertexOffset = MeshSection.MeshDataDescriptor.MeshletVertexTrianglesIndexOffset;
                   Descritptor.SubMeshTriangleOffset = MeshSection.MeshDataDescriptor.MeshletTrianglesOffset;

                   //Descritptor.MatrixMV = Tbx::Matrix4x4f(ModelView);
                   //Descritptor.NormalInverMatrixMV = Descritptor.MatrixMV.Invert().Transpose();
                   const uint64_t shaderKey = reinterpret_cast<uint64_t>(Descritptor.ShaderProgram) >> 4;
                   const uint64_t materialKey = reinterpret_cast<uint64_t>(Descritptor.MaterialDescriptor) >> 4;
                   const uint32_t depthKey = static_cast<uint32_t>(DistanceAABBToCamera * FIXED_POINT_NUMBER);
                   item.SortKey = ComputeSortKey(isOpaque, depthKey, materialKey, shaderKey);

               }
                   break;
               case RenderMode::PathTracing:
                   break;
               default:
                   assert(false);
                   break;
               }


               int d = 0;
           }
       }
   }

   void Renderer::FillListDebugDraw(const RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       // Instanced 
       {
           auto FillPrimitive = [&](const std::vector<DebugDrawContext::DrawPrimitive>& DrawBoxs, size_t PrimitiveIndex, bool isWired)
               {
                   DebugInstanceBuffer.clear();
                   if (DrawBoxs.empty())
                       return;

                   DebugInstanceBuffer.reserve(DrawBoxs.size());

                   for (const auto& Primitive : DrawBoxs)
                   {
                       Color Color(FloatRGBA{
                           Primitive.Color.x, Primitive.Color.y, Primitive.Color.z, 1.0f
                           });
                       Tbx::Matrix4x4f trsV = Tbx::Matrix4x4f(_view.View * Tbx::Trs4x4(Primitive.Origin, Primitive.Euler, Primitive.Size));
                       trsV[15] = std::bit_cast<float>(Color.ToPackedRGBA());
                       DebugInstanceBuffer.emplace_back() = Tbx::Matrix4x4f(trsV);
                   }

                   auto& InstanceBufferBox = m_DebugPrimitiveBuffer[PrimitiveIndex].InstanceBuffer;

                   InstanceBufferBox->UploadData(m_CommandList.get(), DebugInstanceBuffer.data(), DebugInstanceBuffer.size() * sizeof(DebugInstanceBuffer[0]));

                   BufferStateTransition bufferTransition =
                   {
                       .Buffer = InstanceBufferBox.get() ,
                       .Offset = 0,
                       .Size = PC_CORE::WHOLE_SIZE,
                       .updateState = false
                   };

                   m_CommandList->Barrier(RhiResourceState::CopyDst, RhiResourceState::VertexShaderResource, {}, std::span(&bufferTransition, 1));

                   auto& item = DebugDrawList.EmplaceBack();
                   auto& InstanceDebugDraw = item.Data.emplace<DrawDebugInstanced>();
                   InstanceDebugDraw.ShaderProgram = nullptr;
                   InstanceDebugDraw.VertexBuffer = m_DebugPrimitiveBuffer[PrimitiveIndex].VertexBuffer.Get();
                   InstanceDebugDraw.IndexBuffer = m_DebugPrimitiveBuffer[PrimitiveIndex].IndexBuffer.Get();
                   InstanceDebugDraw.InstanceBuffer = m_DebugPrimitiveBuffer[PrimitiveIndex].InstanceBuffer.get();
                   InstanceDebugDraw.IndexFormat = m_DebugPrimitiveBuffer[PrimitiveIndex].IndexBuffer.GetIndexFormat();
                   InstanceDebugDraw.IndexCount = m_DebugPrimitiveBuffer[PrimitiveIndex].IndexBuffer.GetIndexCount();
                   InstanceDebugDraw.InstanceCount = DrawBoxs.size();
                   InstanceDebugDraw.isWired = isWired;
                   item.SortKey = 0;
               };

           for (size_t i = 0; i < RenderingWorldData.DebugDrawPrimitives.size(); i++)
           {
               const DebugDrawContext::PrimitiveType CurrentPrimitive = static_cast<DebugDrawContext::PrimitiveType>(i);
               const auto& data = RenderingWorldData.DebugDrawPrimitives[i];
               bool IsWired = false;
               switch (CurrentPrimitive)
               {
               case DebugDrawContext::PrimitiveType::WireSphere:
               case DebugDrawContext::PrimitiveType::WireBox:
                   //case DebugDrawContext::PrimitiveType::WireCapsule:
                   IsWired = true;
                   break;
               default:
                   break;
               }
               FillPrimitive(data, i, IsWired);
           }
       }
       
       // None Instanced
       {
           for (const auto& F : RenderingWorldData.DebugFrustums)
           {
               auto& item = DebugDrawList.EmplaceBack();
               auto& InstanceDebugDraw = item.Data.emplace<DrawDebug>();
               InstanceDebugDraw.IndexCount = 24; // yes
               InstanceDebugDraw.isWired = F.IsWired;
               InstanceDebugDraw.VP = Tbx::Matrix4x4f(F.FrustumToWorld);
               InstanceDebugDraw.ShaderProgram = DrawDebugShapeFrustum.get();
               /*
               const Color Color(FloatRGBA{
                           F.Color.x, F.Color.y, F.Color.z, 1.0f
                   });
               InstanceDebugDraw.VP[15] = std::bit_cast<float>(Color.ToPackedRGBA());
              */

           }

           if (_view.Flag & RenderView::RenderViewFlagBits::DrawFrustum)
           {
               auto& item = DebugDrawList.EmplaceBack();
               auto& InstanceDebugDraw = item.Data.emplace<DrawDebug>();
               InstanceDebugDraw.IndexCount = 24; // yes
               InstanceDebugDraw.isWired = true;
               InstanceDebugDraw.VP = Tbx::Matrix4x4f(_view.ViewProjectionInv);
               InstanceDebugDraw.ShaderProgram = DrawDebugShapeFrustum.get();
           }
       }
       
   }

   void Renderer::FillSkyBox(const RenderView& _view, const RenderingWorldData& _RenderingWorldData)
   {
       if (!SkyBoxDescriptorSet)
           return;

       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);
       auto& item = Skybox.EmplaceBack();
       Tbx::Matrix4x4f view = Tbx::Matrix4x4f(_view.View);
       view[15] = 1.f;
       view[14] = 0.f;
       view[13] = 0.f;
       view[12] = 0.f;
       const Tbx::Matrix4x4f ViewProjectionCorrected = Tbx::Matrix4x4f(_view.ClipSpaceCorrection) * Tbx::Matrix4x4f(_view.Projection) * view;
       item.Data.emplace<DrawSkyBox>(ViewProjectionCorrected);
   }

   void Renderer::SortList()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       OpaqueList.Sort(std::ranges::less{}, &DrawItem::SortKey);
       TransparentList.Sort(std::ranges::greater{}, &DrawItem::SortKey);
       DebugDrawList.Sort(std::ranges::greater{}, &DrawItem::SortKey);
   }

   size_t Renderer::PickLodCount(const std::vector<double>& LodThreshold, double BoundingSphereRadius, double AABBDistanceToCam, double FovRad)
   {
       if (LodThreshold.empty())
           return 0;

       const double ScreenSize = BoundingSphereRadius / (AABBDistanceToCam * std::tan(FovRad * 0.5));

       for (size_t i = 0; i < LodThreshold.size(); i++)
       {
           if (ScreenSize >= LodThreshold[i])
           {
               return i;
           }
       }
       return LodThreshold.size();
   }

  
   void Renderer::InitDebugResource()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       for (size_t i = 0; i < m_DebugPrimitiveBuffer.size(); i++)
       {
           auto& DebugLayer = m_DebugPrimitiveBuffer[i];
           if (DebugLayer.VertexBuffer.Get() && DebugLayer.IndexBuffer.Get() && DebugLayer.InstanceBuffer.get())
               continue;

           DebugLayer.VertexBuffer = VertexBuffer(m_Rhi);
           DebugLayer.IndexBuffer = IndexBuffer(m_Rhi);
           DebugLayer.InstanceBuffer.reset(m_Rhi.CreateBuffer());

           auto [verticies, indicies] = PC_CORE::DebugDrawContext::GenerateBasePrimitve(static_cast<PC_CORE::DebugDrawContext::PrimitiveType>(i));

           if (verticies.empty() || indicies.empty())
               continue;

           DebugLayer.VertexBuffer
               .SetVerticiesCount(verticies.size())
               .SetVerticiesSize(sizeof(verticies[0]))
               ->SetSizeInBytes(verticies.size() * sizeof(verticies[0]))
               .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
               .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
               .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex | RhiBuffer::BufferUsageFlagBits::ShaderStorage)
               .SetName("Vertex Buffer " + PC_CORE::DebugDrawContext::PrimitiveTypeToString(static_cast<PC_CORE::DebugDrawContext::PrimitiveType>(i)))
               .Build();

           DebugLayer.IndexBuffer
               .SetIndexCount(indicies.size())
               .SetIndexFormat(RhiBuffer::IndexFormat::Uint32)
                ->SetSizeInBytes(indicies.size() * sizeof(indicies[0]))
               .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
               .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Static)
               .SetUsage(RhiBuffer::BufferUsageFlagBits::Index | RhiBuffer::BufferUsageFlagBits::ShaderStorage)
               .SetName("Index Buffer " + PC_CORE::DebugDrawContext::PrimitiveTypeToString(static_cast<PC_CORE::DebugDrawContext::PrimitiveType>(i)))
               .Build();

           DebugLayer.InstanceBuffer
               ->SetSizeInBytes(sizeof(Tbx::Matrix4x4f) * MAX_DEBUG_INSTANCE)
               .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
               .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::PerFrame)
               .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage)
               .SetName("Instance Buffer " + PC_CORE::DebugDrawContext::PrimitiveTypeToString(static_cast<PC_CORE::DebugDrawContext::PrimitiveType>(i)))
               .Build();

           std::scoped_lock _(m_Rhi.GetRhiContext().ResourceUpdateLock());
           m_Rhi.GetRhiContext().ResourceUpdateBranch()->BufferUpload(*DebugLayer.VertexBuffer.Get(), verticies.data(), DebugLayer.VertexBuffer->GetSizeInByte());
           m_Rhi.GetRhiContext().ResourceUpdateBranch()->BufferUpload(*DebugLayer.IndexBuffer.Get(), indicies.data(), DebugLayer.IndexBuffer->GetSizeInByte());
       }
   }

   void Renderer::PrepareInstanceBuffer(const RenderingWorldData& RenderingWorldData)
   {
       m_InstanceBufferCpu.clear();
       const size_t StaticMeshCount = RenderingWorldData.StaticMeshComponentData.size();

       size_t DebugCount = 0;
       for (size_t i = 0; i < RenderingWorldData.DebugDrawPrimitives.size(); i++)
           DebugCount += RenderingWorldData.DebugDrawPrimitives[i].size();
       
       m_InstanceBufferCpu.reserve(StaticMeshCount + DebugCount);
   }

}

