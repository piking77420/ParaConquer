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
    }

   void Renderer::Build(const RenderView& _View, const std::function<void(RenderGraph&)>& InitRenderGraphFunction)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);
       m_RenderGraph.Clear();
       InitRenderGraphFunction(m_RenderGraph);
       InitShaders(); // should be call in constructor however shader creation are build after app cronstructor
 
       RendererPassBuildContext buildContext(*m_CommandList, m_Rhi, m_RenderGraph, _View, *this);
       m_RenderGraph.Build(buildContext);
   }

   void Renderer::Excute(RenderView& _View, const RenderingWorldData& RenderingWorldData)
   {
       BuildDrawLists(_View, RenderingWorldData);

       RendererPassExecuteContext executeContext(*m_CommandList, m_Rhi, m_RenderGraph, _View, *this, RenderingWorldData);
       m_RenderGraph.Execute(executeContext, _View);
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
               .SetFinalImageState(RhiResourceState::FragmentShaderResource);

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
               .SetType(RhiShaderProgram::PipelineType::Graphic)
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
               .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::FragmentShaderResource);

           colorLinearPass
               ->CreateSubPass()
               .SetType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));

           colorLinearPass
               ->SetName("ColorLinearPass")
               .Build();
       }

       {
           colorLinearPassDepth.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = colorLinearPassDepth
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::FragmentShaderResource);

           const RenderPassAttachementDescriptor& DepthAttachement = colorLinearPassDepth
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S01)
               .SetRhiFormat(RhiFormat::D24UnormS8Uint)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::DepthStencilWrite);

           colorLinearPassDepth
               ->CreateSubPass()
               .SetType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget))
               .SetDepthAttachementRef(AttachementRef(DepthAttachement, RhiResourceState::DepthStencilWrite));

           colorLinearPassDepth
               ->SetName("ColorLinearPassDepth")
               .Build();
       }
      
   }

   void Renderer::InitShaders()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Compute, ResourceManager::Get<ShaderSourceBinary>("Aces.cs.hlsl.binary")->GetCode() },
           };
           toneMapAces.reset(m_Rhi.CreateRhiShaderProgram());
           toneMapAces
               ->SetPipelineType(RhiShaderProgram::PipelineType::Compute)
               .SetShaderModules(shaderModules)
               .SetName("ToneMapAces")
               .Build();
       }

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
               .SetRenderPass(*colorLinearPass)
               .SetName("DrawQuadTriangle")
               .Build();
       }

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Vertex, ResourceManager::Get<ShaderSourceBinary>("Forward.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Pixel, ResourceManager::Get<ShaderSourceBinary>("Forward.ps.hlsl.binary")->GetCode() }
           };

           opaqueFowardShader.reset(m_Rhi.CreateRhiShaderProgram());
           opaqueFowardShader
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetCullMode(RhiShaderProgram::CullModeFlagBits::CullBack)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*forwardPass)
               .SetDepthTest(true)
               .SetDepthWrite(true)
               .SetVertexAttributeDescriptions(StaticMeshVertex::GetAttributeDescriptions(0))
               .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) })
               .SetName("Opaque FowardShader")
               .Build();


           constexpr PC_CORE::RhiShaderProgram::BlendState blenstate =
           {
               .ColorSrcFactor = PC_CORE::BlendFactor::SrcAlpha,
               .ColorDstFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
               .ColorOp = PC_CORE::BlendOp::Add,

               .AlphaSrcFactor = PC_CORE::BlendFactor::One,
               .AlphaDstFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
               .AlphaOp = PC_CORE::BlendOp::Add,

               .BlendMask = PC_CORE::ColorComponent::ColorComponentRGBA
           };
           transparentForwardShader.reset(m_Rhi.CreateRhiShaderProgram());
           transparentForwardShader
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*forwardPass)
               .SetDepthTest(true)
               .SetDepthWrite(true)
               .SetVertexAttributeDescriptions(StaticMeshVertex::GetAttributeDescriptions(0))
               .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) })
               .SetName("Transparent FowardShader")
               .SetBlendState(blenstate)
               .Build();

       }
       
       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Amp, ResourceManager::Get<ShaderSourceBinary>("MeshShaderMeshlet.as.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Mesh, ResourceManager::Get<ShaderSourceBinary>("MeshShaderMeshlet.ms.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Pixel, ResourceManager::Get<ShaderSourceBinary>("MeshShaderMeshlet.ps.hlsl.binary")->GetCode() }
           };

           meshShaderMeshlet.reset(m_Rhi.CreateRhiShaderProgram());
           meshShaderMeshlet
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*forwardPass)
               .SetDepthWrite(true)
               .SetDepthTest(true)
               .SetName("Triangle MeshShader")
               .Build();
       }

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Vertex, ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangle.vs.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Pixel, ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangle.ps.hlsl.binary")->GetCode() },
           };

           DrawMeshTriangle.reset(m_Rhi.CreateRhiShaderProgram());
           DrawMeshTriangle
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*colorLinearPassDepth)
               .SetVertexAttributeDescriptions({ VertexAttributeDescription{
                .Binding = 0,
                .Location = 0,
                .Format = RhiFormat::R32G32B32A32Sfloat,
                .Offset = offsetof(StaticMeshVertex, Position)
                } })
               .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) })
               .SetDepthWrite(true)
               .SetDepthTest(true)
               .SetName("Draw Mesh Triangle")
               .Build();
       }

       {
           const std::vector<RhiShaderProgram::ShaderModule> shaderModules
           {
               { RhiShaderProgram::ShaderStageTypeBits::Amp, ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangleMeshlet.as.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Mesh, ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangleMeshlet.ms.hlsl.binary")->GetCode() },
               { RhiShaderProgram::ShaderStageTypeBits::Pixel, ResourceManager::Get<ShaderSourceBinary>("DrawMeshTriangleMeshlet.ps.hlsl.binary")->GetCode() }
           };

           DrawMeshTriangleMeshlet.reset(m_Rhi.CreateRhiShaderProgram());
           DrawMeshTriangleMeshlet
               ->SetPipelineType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementCount(1)
               .SetShaderModules(shaderModules)
               .SetRenderPass(*colorLinearPassDepth)
               .SetVertexAttributeDescriptions({ VertexAttributeDescription{
                .Binding = 0,
                .Location = 0,
                .Format = RhiFormat::R32G32B32A32Sfloat,
                .Offset = offsetof(StaticMeshVertex, Position)
                } })
               .SetVertexInputBindingDescritions({ StaticMeshVertex::GetVertexBindingDescription(0) })
               .SetDepthWrite(true)
               .SetDepthTest(true)
               .SetName("Draw Mesh Triangle Meshlet")
               .Build();
       }
   }

   void Renderer::BuildDrawLists(RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       OpaqueList.Clear();
       TransparentList.Clear();
       FillListStaticMesh(_view, RenderingWorldData);
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
           const std::vector<MeshDrawCommand>& DrawCommands = StaticMeshData.DrawCommands;

           // Pick Lod
           uint32_t LODIndex = 0;
           const MotionCore::Aabb<double> AABBW = StaticMeshComponentData.StaticMesh->GetAabb().GetTransformed(StaticMeshComponentData.WorldMatrix);
           const double DistanceAABBToCamera = (AABBW.GetCenter() - _view.ViewPosition).Magnitude();
           const double BoundingSphereRadius = (AABBW.GetSize() * 0.5).Magnitude();

           const Tbx::Matrix4x4d ModelView = _view.View * StaticMeshComponentData.WorldMatrix;
  
           if (!StaticMesh->GetLodThreshold().empty())
           {
               LODIndex = PickLodCount(StaticMesh->GetLodThreshold(), BoundingSphereRadius, DistanceAABBToCamera, _view.Fov);
           }

           const auto& Lod = StaticMeshData.MeshLods[LODIndex];

           for (const auto& Dcmd : DrawCommands)
           {
               const MeshSection& MeshSection = Lod.MeshesSections[Dcmd.MeshSectionIndex];
               const Material* Material = StaticMeshComponentData.Materials.at(MeshSection.MaterialIndex);
               if (!Material)
                   continue; // to do get dummy mat

               const bool isOpaque = Material->GetMaterialType() == MaterialType::Opaque;
               DrawList& DrawList = isOpaque ? OpaqueList : TransparentList;
               DrawItem& item = DrawList.EmplaceBack();

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
                   Descritptor.MatrixMV = Tbx::Matrix4x4f(ModelView);
                   Descritptor.NormalInverMatrixMV = Descritptor.MatrixMV.Invert().Transpose();

                   const uint64_t shaderKey = reinterpret_cast<uint64_t>(Descritptor.ShaderProgram) >> 4;
                   const uint64_t materialKey = reinterpret_cast<uint64_t>(Descritptor.MaterialDescriptor) >> 4;
                   const uint32_t depthKey = static_cast<uint32_t>(DistanceAABBToCamera * FIXED_POINT_NUMBER);

                   if (isOpaque)
                   {
                       item.SortKey =
                           ((shaderKey & 0xFFFF) << 48) |
                           ((materialKey & 0xFFFF) << 32) |
                           depthKey; // depth is lsb
                   }
                   else
                   {
                       // Reverse depth so that larger values correspond to closer objects.
                       // When sorting ascending, this results in back-to-front ordering.
                       item.SortKey =
                           ((std::numeric_limits<uint32_t>::max() - depthKey) << 32) |
                           (materialKey << 16) |
                           shaderKey;
                   }
               }
                   break;
               case RenderMode::ClusterBased:
               {
                   DrawStaticMeshMeshlet& Descritptor = item.Data.emplace<DrawStaticMeshMeshlet>();

                   Descritptor.ShaderProgram = meshShaderMeshlet.get();
                   Descritptor.MaterialDescriptor = nullptr;
                   Descritptor.MeshletDescriptor = StaticMesh->GetMeshletDescriptor(LODIndex);

                   Descritptor.VertexOffset = MeshSection.MeshDataDescriptor.VertexOffset;
                   Descritptor.MeshletOffset = MeshSection.MeshDataDescriptor.MeshetOffset;
                   Descritptor.MeshletCount = MeshSection.MeshDataDescriptor.MeshetCount;
                   Descritptor.SubMeshTriangleVertexOffset = MeshSection.MeshDataDescriptor.MeshletVertexTrianglesIndexOffset;
                   Descritptor.SubMeshTriangleOffset = MeshSection.MeshDataDescriptor.MeshletTrianglesOffset;

                   Descritptor.MatrixMV = Tbx::Matrix4x4f(ModelView);
                   Descritptor.NormalInverMatrixMV = Descritptor.MatrixMV.Invert().Transpose();
                   const uint64_t shaderKey = reinterpret_cast<uint64_t>(Descritptor.ShaderProgram) >> 4;
                   const uint64_t materialKey = reinterpret_cast<uint64_t>(Descritptor.MaterialDescriptor) >> 4;
                   const uint32_t depthKey = static_cast<uint32_t>(DistanceAABBToCamera * FIXED_POINT_NUMBER);

                   // all opaque for now
                   item.SortKey =
                       ((shaderKey & 0xFFFF) << 48) |
                       ((materialKey & 0xFFFF) << 32) |
                       depthKey; // depth is lsb
               }
                   break;
               case RenderMode::PathTracing:
                   break;
               default:
                   assert(false);
                   break;
               }
           }
       }
   }

   void Renderer::SortList()
   {
       PERF_REGION_SCOPED;
       PERF_REGION_COLOR(PerfRegion::Rendering);

       OpaqueList.Sort(std::ranges::less{}, &DrawItem::SortKey);
       TransparentList.Sort(std::ranges::greater{}, &DrawItem::SortKey);
   }

   size_t Renderer::PickLodCount(const std::vector<double>& LodThreshold, double BoundingSphereRadius, double AABBDistanceToCam, double FovRad) const
   {
       if (LodThreshold.empty())
           return 0;

       double ScreenSize = BoundingSphereRadius / (AABBDistanceToCam * std::tan(FovRad * 0.5));

       for (size_t i = 0; i < LodThreshold.size(); i++)
       {
           if (ScreenSize >= LodThreshold[i])
           {
               return i;
           }
       }
       return LodThreshold.size();
   }

}

