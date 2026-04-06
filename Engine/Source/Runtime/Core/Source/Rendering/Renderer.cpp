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

       RendererPassBuildContext buildContext(*m_CommandList, m_Rhi, m_RenderGraph, _View, *this);
       m_RenderGraph.Build(buildContext);

   }

   void Renderer::Excute(RenderView& _View, const RenderingWorldData& RenderingWorldData)
   {
       RendererPassExecuteContext executeContext(*m_CommandList, m_Rhi, m_RenderGraph, _View, *this, RenderingWorldData);

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
           drawTextureQuadPass.reset(m_Rhi.CreateRenderPass());

           const RenderPassAttachementDescriptor& renderTragetSlot = drawTextureQuadPass
               ->CreateAttachment()
               .SetAttachementSlot(AttachementSlot::S00)
               .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
               .SetSampleCount(1)
               .SetLoadOp(LoadOperation::Clear)
               .SetStoreOp(StoreOperation::Store)
               .SetInitialImageState(RhiResourceState::Undefined)
               .SetFinalImageState(RhiResourceState::FragmentShaderResource);

           drawTextureQuadPass
               ->CreateSubPass()
               .SetType(RhiShaderProgram::PipelineType::Graphic)
               .SetAttachementRef(AttachementRef(renderTragetSlot, RhiResourceState::RenderTarget));

           drawTextureQuadPass
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
               .SetRenderPass(*drawTextureQuadPass)
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
   }

   void Renderer::BuildDrawLists(RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {
       m_OpaqueList.Clear();
       m_TransparentList.Clear();
       // TODO MAYCOUNT FOR RESERVE
       FillListStaicMesh(_view, RenderingWorldData);
       SortList();
   }

   void Renderer::FillListStaicMesh(RenderView& _view, const RenderingWorldData& RenderingWorldData)
   {

       for (const auto& StaticMeshComponentData : RenderingWorldData.StaticMeshComponentData)
       {
           const StaticMesh* StaticMesh = StaticMeshComponentData.StaticMesh;
           const StaticMeshData& StaticMeshData = StaticMesh->GetStaticMeshData();
           const std::vector<MeshDrawCommand>& DrawCommands = StaticMeshData.DrawCommands;

           // Pick Lod
           uint32_t LODIndex = 0;
           MotionCore::Aabb<double> AABBW = StaticMeshComponentData.StaticMesh->GetAabb().GetTransformed(StaticMeshComponentData.WorldMatrix);
           double DistanceToCamera = (AABBW.GetCenter() - _view.ViewPosition).Magnitude();
           double BoundingSphereRadius = (AABBW.GetSize() * 0.5).Magnitude();

           if (!StaticMesh->GetLodThreshold().empty())
           {
               const Tbx::Matrix4x4d ModelView = _view.View * StaticMeshComponentData.WorldMatrix;
               const Tbx::Matrix4x4d NormalInvMatrixView = ModelView.Invert().Transpose();
               LODIndex = PickLodCount(StaticMesh->GetLodThreshold(), BoundingSphereRadius, DistanceToCamera, _view.Fov);
           }

           const auto& Lod = StaticMeshData.MeshLods[LODIndex];

           for (const auto& Dcmd : DrawCommands)
           {
               const MeshSection& MeshSection = Lod.MeshesSections[Dcmd.MeshSectionIndex];
               const Material* Material = StaticMeshComponentData.Materials.at(MeshSection.MaterialIndex);
               if (!Material)
                   continue; // to do get dummy mat

               DrawList& DrawList = Material->GetMaterialType() == MaterialType::Opaque ? m_OpaqueList : m_TransparentList;
               DrawItem item;

               if (StaticMeshComponentData.UseMeshlet)
               {
                   DrawStaticMeshTriangle& Descritptor = item.emplace<DrawStaticMeshTriangle>();
                   Descritptor.VertexBuffer = StaticMesh->GetVertexBuffer(LODIndex).Get();
                   Descritptor.IndexBuffer = StaticMesh->GetIndexBuffer(LODIndex).Get();
                   Descritptor.VertexOffset = MeshSection.MeshDataDescriptor.VertexOffset;
                   Descritptor.IndexOffset = MeshSection.MeshDataDescriptor.IndicesOffset;
                   Descritptor.IndexCount = MeshSection.MeshDataDescriptor.IndicesCount;
                   Descritptor.DitanceAABBToCam = DistanceToCamera;
               }
               else
               {
                   DrawStaticMeshMeshlet& Descritptor = item.emplace<DrawStaticMeshMeshlet>();

                   //Descritptor.DitanceAABBToCam = DistanceToCamera;
               }

               DrawList.AddItem(item);
           }

       }
   }

   void Renderer::SortList()
   {
       /*
       m_OpaqueList.Sort([](const DrawItem& A, const DrawItem& B)
           {
               return std::visit([&](auto&& ItemA) ->bool {
                   return std::visit([&](auto&& ItemB) ->bool {
                       if (ItemA.ShaderProgram != ItemB.ShaderProgram)
                           return ItemA.ShaderProgram < ItemB.ShaderProgram;


                       }, B);
                   }, A);

              

               if (a.materialSet != b.materialSet)
                   return a.materialSet < b.materialSet;

               if (a.vertexBuffer != b.vertexBuffer)
                   return a.vertexBuffer < b.vertexBuffer;

               return a.indexBuffer < b.indexBuffer;
           });*/
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

