#include "rendering/renderer.hpp"

#include "low_renderer/rhi.hpp"
#include "low_renderer/vertex.hpp"
#include "resources/resource_manager.hpp"
#include <thread>
#include "time/core_time.hpp"
#include "math/matrix_transformation.hpp"
#include "rendering/light.hpp"
#include "resources/texture_2d.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "rendering/render_passes/render_pass.hpp"

#include <perf_region.hpp>

#include "math/toolbox_typedef.hpp"
#include "resources/shader_source_binary.hpp"

using namespace PC_CORE;


void Renderer::InitRenderSystem()
{
    PERF_REGION_SCOPED;
    Level& level = World::GetWorld()->level;

    rendererSystem = level.RegisterSystem<RendererSystem>();

    rendererSystem->staticMeshSignature.set(level.GetComponentTypeBit<Transform>(), true);
    rendererSystem->staticMeshSignature.set(level.GetComponentTypeBit<StaticMesh>(), true);
    rendererSystem->AddSignature(rendererSystem->staticMeshSignature);

    rendererSystem->dirLightSignature.set(level.GetComponentTypeBit<Transform>(), true);
    rendererSystem->dirLightSignature.set(level.GetComponentTypeBit<DirLight>(), true);
    rendererSystem->AddSignature(rendererSystem->dirLightSignature);

}

void Renderer::Init()
{
    PERF_REGION_SCOPED;

    m_RhiContext = Rhi::GetRhiContext();
    constexpr CommandListCreateInfo commandListCreateInfo =
    {
        ._commandPoolFamily = CommandPoolFamily::Graphics
    };

    primaryCommandList = PC_CORE::Rhi::CreateCommandList(commandListCreateInfo);

    CreateRenderPasss();
    CreateShaders();
    CreateThirdPartyResources();
    CreateDescriptorSets();
   
    InitRenderSystem();


    m_DebugDrawContext = std::make_unique<DebugDrawContext>(this);
}

void Renderer::BeginDraw(Window* _window)
{
    PERF_REGION_SCOPED;

    m_RhiContext->swapChain->GetSwapChainImageIndex(_window);


    primaryCommandList->Reset();
    primaryCommandList->BeginRecordCommands();

    m_DebugDrawContext->Prepare();
    QueryWorldData(World::GetWorld());
    sceneLightsBuffer
        ->Fecth();
        
}



void Renderer::UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext)
{
    PERF_REGION_SCOPED;

    
    currentRenderingContext = &renderingContext;
    SceneBufferGPU& sceneBufferGpu = sceneBufferGPU;

    sceneBufferGpu.time = PC_CORE::Time::GetTime();
    sceneBufferGPU.deltatime = PC_CORE::Time::DeltaTime();
    
    Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
                                                renderingContext.lowLevelCamera.front, renderingContext.lowLevelCamera.up);
    Tbx::Matrix4x4f projection = Tbx::PerspectiveMatrixFlipYAxis<float>(renderingContext.lowLevelCamera.fov,
                                                                        renderingContext.lowLevelCamera.aspect,
                                                                        renderingContext.lowLevelCamera.near,
                                                                        renderingContext.lowLevelCamera.far);
    sceneBufferGPU.vp = projection * view;
    sceneBufferGPU.view = view;
    sceneBufferGPU.proj = projection;
    sceneBufferGPU.viewInv = sceneBufferGPU.view.Invert();
    sceneBufferGPU.projInv = sceneBufferGPU.proj.Invert();
    sceneBufferGPU.vpInv = sceneBufferGPU.vp.Invert();
    sceneBufferGPU.cameraNear = renderingContext.lowLevelCamera.near;
    sceneBufferGPU.cameraFar = renderingContext.lowLevelCamera.far;
    sceneBufferGPU.cameraPos = renderingContext.lowLevelCamera.position;

    cameraUniformBuffer.Update(&sceneBufferGPU, sizeof(sceneBufferGPU));
}


void Renderer::DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext,
                                      World* _world)
{
    
    PERF_REGION_SCOPED;
    UpdateCameraUniformBuffer(renderingContext);

    m_CurrentWorld = _world;
    const ViewportInfo viewportInfo =
    {
        .transform = {0, 0},
        .size = {
            static_cast<float>(renderingContext.renderingContextSize.x),
            static_cast<float>(renderingContext.renderingContextSize.y)
        },
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .scissorsOff = {0, 0},
        .scissorsextent = {renderingContext.renderingContextSize.x, renderingContext.renderingContextSize.y}
    };

    primaryCommandList->SetViewPort(viewportInfo);

    ForwardPass(renderingContext, viewportInfo);
    FinalPass(renderingContext, viewportInfo);
}



void Renderer::SwapBuffers(Window* _window)
{
    PERF_REGION_SCOPED;

    
    std::shared_ptr<PC_CORE::SwapChain> swapChain = RhiContext::GetContext().swapChain;
    swapChain->BeginSwapChainRenderPass(primaryCommandList.get());

    primaryCommandList->ExucuteFetchCommand();
    swapChain->EndSwapChainRenderPass(primaryCommandList.get());
    primaryCommandList->EndRecordCommands();
    
    m_RhiContext->swapChain->Present(primaryCommandList.get(), _window);
    Rhi::NextFrame();
}

void Renderer::QueryWorldData(World* world)
{
    PERF_REGION_SCOPED;
    
    Level& level = world->level;
    for (const auto& it : *rendererSystem->GetEntityIdList(rendererSystem->dirLightSignature))
    {
        QueryLightDirData(level.GetComponent<DirLight>(it), level.GetComponent<Transform>(it));
    }

}

void Renderer::QueryLightDirData(DirLight& dirLight, Transform& transform)
{
    sceneLightsBuffer
        ->sceneLightData.ambiant = dirLight.ambiant;
    sceneLightsBuffer
        ->sceneLightData.color = dirLight.color;
    sceneLightsBuffer
        ->sceneLightData.intensity = dirLight.intensity;
    sceneLightsBuffer
        ->sceneLightData.direction = Tbx::Quaternionf::ToEulerAngles(transform.rotation.quaternion).Normalize();
}


void Renderer::DrawStaticMesh(PC_CORE::Transform& _transform, PC_CORE::StaticMesh& _staticMesh)
{
    PERF_REGION_SCOPED;

    if (_staticMesh.material.expired() || _staticMesh.mesh.expired())
        return;

    
    // Compute Matrix
    Tbx::Matrix4x4d modelMatrixd[2];

    Tbx::Vector3d d = static_cast<Tbx::Vector3d>(currentRenderingContext->lowLevelCamera.position);
    modelMatrixd[0] = Tbx::Trs4x4<double>(_transform.position - d , static_cast<Tbx::Quaterniond>(_transform.rotation.quaternion),
        _transform.scale);

    modelMatrixd[1] = modelMatrixd[0].Invert().Transpose();

    Tbx::Matrix4x4f modelMatrixf[2];
    modelMatrixf[0] = modelMatrixd[0];
    modelMatrixf[1] = modelMatrixd[1];

    Material* material = _staticMesh.material.lock().get();
    Mesh* mesh = _staticMesh.mesh.lock().get();

    // Send Data
    primaryCommandList->BindDescriptorSet(m_ForwardShader.lock().get(), material->GetDescriptorSet(), MATERIAL_DESCRIPTOR_SET, 1);
    primaryCommandList->PushConstant(m_ForwardShader.lock().get(), "PushConstants", &modelMatrixf,
                                     sizeof(Tbx::Matrix4x4f) * 2);
    primaryCommandList->BindVertexBuffer(*mesh->vertexBuffer.GetRhiBuffer(), 0, 1);
    primaryCommandList->BindIndexBuffer(*mesh->indexBuffer.GetRhiBuffer(), 0);
    primaryCommandList->DrawIndexed(mesh->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
}



void Renderer::DrawSkyBox()
{
    primaryCommandList->BindProgram(m_CubeMapShader.lock().get());
    primaryCommandList->BindDescriptorSet(m_CubeMapShader.lock().get(), descriptorSetsSkybox.cameraDescriptorSet, SCENE_DESCRIPTOR_SET, 1);
    primaryCommandList->BindDescriptorSet(m_CubeMapShader.lock().get(), descriptorSetsSkybox.cubeMapDescriptorSet, ENVIRONEMENT_DESCRIPTOR_SET, 1);
    primaryCommandList->BindVertexBuffer(*m_CubeMesh->vertexBuffer.GetRhiBuffer(), 0, 1);
    primaryCommandList->BindIndexBuffer(*m_CubeMesh->indexBuffer.GetRhiBuffer(), 0);
    primaryCommandList->DrawIndexed(m_CubeMesh->indexBuffer.GetIndexCount(), 1, 0, 0, 0);
}


void Renderer::ForwardPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo)
{
    ClearValueFlags clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor |
        ClearValueFlags::ClearValueDepth);

    std::array<Tbx::Vector4f, 1> clearValues = {
        Tbx::Vector4f(0, 0, 0, 0.f),
    };

    const BeginRenderPassInfo beginRenderPassInfo =
    {
        .renderPass = forwardPass,
        .frameBuffer = _renderingContext.gbufferFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {_renderingContext.renderingContextSize.x, _renderingContext.renderingContextSize.y},
        .clearValueFlags = clearValueFlags,
        .clearColor = clearValues.data(),
        .clearValueCount = clearValues.size(),
        .clearDepth = 1.f
    };


    primaryCommandList->BeginDebugLabel("Forward Pass", FORWARD_DEBUG_COLOR);
    primaryCommandList->BeginRenderPass(beginRenderPassInfo);

    primaryCommandList->BindProgram(m_ForwardShader.lock().get());


    primaryCommandList->SetViewPort(_viewportInfo);
    primaryCommandList->BindDescriptorSet(m_ForwardShader.lock().get(), m_ShaderProgramSceneDescriptorSet, SCENE_DESCRIPTOR_SET, 1);

    primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleList);

    Level& level = World::GetWorld()->level;
    // draw all static mesh
    for (const auto& it : *rendererSystem->GetEntityIdList(rendererSystem->staticMeshSignature))
        DrawStaticMesh(level.GetComponent<Transform>(it),
            level.GetComponent<StaticMesh>(it));

    DrawSkyBox();
#ifdef WITH_EDITOR
    for (auto& it : UserCustomForwardPass)
        it(primaryCommandList.get(), *currentRenderingContext);
    m_DebugDrawContext->DrawDebugPrimitive(primaryCommandList.get(), _renderingContext);
#endif
    primaryCommandList->EndRenderPass();

    primaryCommandList->EndDebugLabel();
}

void Renderer::FinalPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo)
{
    std::array<Tbx::Vector4f, 2> clearValues2 = {
       Tbx::Vector4f(0, 0, 0, 0.f),
       Tbx::Vector4f(0, 0, 0, 0.f),
    };

    const BeginRenderPassInfo drawToViewport =
    {
        .renderPass = drawToFinalViewPort,
        .frameBuffer = _renderingContext.finalImageFrameBuffer,
        .renderOffSet = {0, 0},
        .extent = {_renderingContext.renderingContextSize.x, _renderingContext.renderingContextSize.y},
        .clearValueFlags = static_cast<ClearValueFlags>(ClearValueFlags::ClearValueColor),
        .clearColor = clearValues2.data(),
        .clearValueCount = clearValues2.size(),
        .clearDepth = 0.f,
        .clearStencil = 0.f
    };


    primaryCommandList->BeginDebugLabel("Final Pass", FINAL_RENDER_PASS_DEBUG_COLOR);
    primaryCommandList->BeginRenderPass(drawToViewport);
    primaryCommandList->BindProgram(m_DrawTextureScreenQuadShader.lock().get());
    primaryCommandList->SetPrimitiveTopology(PrimitiveTopology::PrimitiveTopologyTriangleStrip);

    primaryCommandList->BindDescriptorSet(m_DrawTextureScreenQuadShader.lock().get(), _renderingContext.viewPortDescriptorSet, 0, 1);
    primaryCommandList->Draw(4, 1, 0, 0);

    primaryCommandList->EndRenderPass();
    primaryCommandList->EndDebugLabel();
}



#pragma region CreateRenderPasss

void Renderer::CreateRenderPasss()
{
    PERF_REGION_SCOPED;

    // Forward
    {
        PERF_REGION_SCOPED_NAMED("Create Forward RenderPass");

        std::vector<RenderPassAttachementDescriptor> colorAttachement;
        colorAttachement.resize(1);

        colorAttachement[0] =
        {
        .attachmentType = AttachmentType::Color,
        .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
        .sampleCount = 1,
        .load = LoadOperation::Clear,
        .store = StoreOperation::Store,
        .stencilLoad = LoadOperation::DontCare,
        .stencilStore = StoreOperation::DontCare,
        };

        RenderPassAttachementDescriptor depthAttachement =
        {
           .attachmentType = AttachmentType::Depth,
           .format = PC_CORE::RHIFormat::D32_SFLOAT,
           .sampleCount = 1,
           .load = LoadOperation::Clear,
           .store = StoreOperation::Store,
           .stencilLoad = LoadOperation::DontCare,
           .stencilStore = StoreOperation::DontCare,
        };

        std::vector<SubPassDescription> subPassDescriptions;
        subPassDescriptions.resize(1);

        subPassDescriptions[0] =
        {
        .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
        .colorAttachementDescriptorIndicies = {0},
        .subPassDependcies =
        {
            .srcStageMask = static_cast<PipelineStageFlags>(
                PipelineStageFlagBits::ColorAttachmentOutput | PipelineStageFlagBits::EarlyFragmentTests
            ),
            .dstStageMask = static_cast<PipelineStageFlags>(
                PipelineStageFlagBits::FragmentShader
            ),
            .srcAccessMask = {}, // you can set this to ColorAttachmentWrite or DepthStencilAttachmentWrite if needed
            .dstAccessMask = static_cast<AccessFlags>(
                AccessFlagBits::ShaderRead
            )
        },
        .useDepth = true,
        };

        PC_CORE::RenderPassDescriptor renderPassDescriptor =
        {
            .colorAttachement = colorAttachement,
            .depthAttachment = &depthAttachement,
            .subPasses = subPassDescriptions
        };

        forwardPass = Rhi::CreateRenderPass(renderPassDescriptor);

    }

    // Draw To Final Viewport
    {
        PERF_REGION_SCOPED_NAMED("Create Draw To Final Viewport");
        drawToFinalViewPort = Rhi::CreateRenderPass(PC_CORE::RHIFormat::R8G8B8A8_UNORM, Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount());
    }
}


#pragma endregion CreateRenderPasss

#pragma region CreateShaders
void Renderer::CreateShaders()
{
    PERF_REGION_SCOPED;

    {
        PERF_REGION_SCOPED_NAMED("Forward Shader");
        constexpr RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::Back,
            .frontFace = FrontFace::CounterClockwise
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
            .vertexAttributeDescriptions = Vertex::GetAttributeDescriptions(0),
            .enableDepthTest = true,
        };

        const SourceList sources =
        {
            {
                ShaderStageType::VERTEX,
                ResourceManager::Get<ShaderSourceBinary>("forward_spv.vert"),
            },
            {
                ShaderStageType::FRAGMENT,
                ResourceManager::Get<ShaderSourceBinary>("forward_spv.frag")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
        .shaderGraphicPointInfo = shaderGraphicPointInfo,
        .sourceList = sources,
        .renderPass = forwardPass.get()
        };

        m_ForwardShader = ResourceManager::Create<GraphicShader>("ForwardShader", graphicShaderProgramCreateInfo);
    }

    // SkyBox Shader
    {
        PERF_REGION_SCOPED_NAMED("SkyBox Shader");

        const RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::None,
            .frontFace = FrontFace::CounterClockwise,
            .multiSampleRasterization = 1
        };

        VertexAttributeDescription vertexAttributeDescription =
        {
        .binding = 0,
        .location = 0,
        .format = RHIFormat::R32G32B32_SFLOAT,
        .offset = 0
        };

        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .depthCompareOp = CompareOp::LESS_OR_EQUAL,
            .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
            .vertexAttributeDescriptions = {Vertex::GetAttributeDescriptions(0)},
            .enableDepthTest = true,
        };

        const SourceList source =
        {
            {
                ShaderStageType::VERTEX,
                ResourceManager::Get<ShaderSourceBinary>("cube_map_skybox_spv.vert")
            },
            {
                ShaderStageType::FRAGMENT,
                ResourceManager::Get<ShaderSourceBinary>("cube_map_skybox_spv.frag")
            }
        };

        const ShaderInfo shaderInfo =
        {
            .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
            .shaderInfoData = shaderGraphicPointInfo,
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
          .shaderGraphicPointInfo = shaderGraphicPointInfo,
          .sourceList = source,
          .renderPass = forwardPass.get(),
        };


        m_CubeMapShader = ResourceManager::Create<PC_CORE::GraphicShader>("SkyboxShader", graphicShaderProgramCreateInfo);

    }

    // Draw to final viewport
    {
        PERF_REGION_SCOPED_NAMED("CreateDrawToFinalViewport Programm");

        const RasterizerInfo rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::None,
            .frontFace = FrontFace::CounterClockwise,
            .multiSampleRasterization = Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount()
        };


        const ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
            .rasterizerInfo = rasterizerInfo,
            .vertexInputBindingDescritions = {},
            .vertexAttributeDescriptions = {},
            .enableDepthTest = false,
        };

        const SourceList sources =
        {
            {
                ShaderStageType::VERTEX,
                ResourceManager::Get<ShaderSourceBinary>("draw_texture_screen_quad_spv.vert"),
            },
            {
                ShaderStageType::FRAGMENT,
                 ResourceManager::Get<ShaderSourceBinary>("draw_texture_screen_quad_spv.frag")
            }
        };

        const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
          .shaderGraphicPointInfo = shaderGraphicPointInfo,
          .sourceList = sources,
          .renderPass = drawToFinalViewPort.get()
        };


        m_DrawTextureScreenQuadShader = ResourceManager::Create<GraphicShader>("DrawQuadShader", graphicShaderProgramCreateInfo);
    }
}

#pragma endregion CreateShaders


#pragma region CreateThirdPartyResources

void Renderer::CreateThirdPartyResources()
{
    PERF_REGION_SCOPED;
    {
        PERF_REGION_SCOPED_NAMED("Create Cube Map");
        std::array<std::string, 6> maps
        {
            "assets/textures/skybox/right.jpg",
            "assets/textures/skybox/left.jpg",
            "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg",
            "assets/textures/skybox/front.jpg",
            "assets/textures/skybox/back.jpg",
        };
        m_Cubemap = ResourceManager::Create<Texture3D>("BasicCubemap", maps);

    }
    {
        sceneLightsBuffer = std::make_unique<SceneLightsBuffer>();

        cameraUniformBuffer = UniformBuffer(&sceneBufferGPU, sizeof(sceneBufferGPU), MemoryUsage::Dynamic);

        m_CubeMesh = ResourceManager::Get<Mesh>("cube.obj");
    }
}
#pragma endregion CreateThirdPartyResources

#pragma region CreateDescriptorSets

void Renderer::CreateDescriptorSets()
{
    PERF_REGION_SCOPED;
    /////////////////////////////////////////////////
    UniformBufferDescriptor cameraBufferDescritptor
    {
        .buffer = &cameraUniformBuffer,
    };

    UniformBufferDescriptor lightData
    {
        .buffer = &sceneLightsBuffer
        ->uniformBuffer,
    };

    ImageSamperDescriptor skyboxCubeMapDescritptor
    {
        .sampler = ResourceManager::Get<Sampler>("LinearRepeat").get(),
        .texture = m_Cubemap.get()
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets;

    {
        PERF_REGION_SCOPED_NAMED("Create Forward Shader DescriptorSet");

        descriptorSets =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                &cameraBufferDescritptor,
                nullptr,
            },
            {
                ShaderProgramDescriptorType::UniformBuffer,
                LIGHTDATA_BINDING,
                &lightData,
                nullptr,
            },
            {
                ShaderProgramDescriptorType::CombineImageSampler,
                FORWARD_SKYBOX_CUBEMAP,
                nullptr,
                &skyboxCubeMapDescritptor,
            }
        };

        m_ForwardShader.lock()->AllocDescriptorSet(&m_ShaderProgramSceneDescriptorSet, SCENE_DESCRIPTOR_SET);
        m_ShaderProgramSceneDescriptorSet->WriteDescriptorSets(descriptorSets);
    }

  
    {
        PERF_REGION_SCOPED_NAMED("Skybox Shader DescriptorSets");

        descriptorSets =
        {
             {
                 ShaderProgramDescriptorType::UniformBuffer,
                 CAMERA_BINDING,
                 &cameraBufferDescritptor,
                 nullptr,
             },
        };
        m_CubeMapShader.lock()->AllocDescriptorSet(&descriptorSetsSkybox.cameraDescriptorSet, SCENE_DESCRIPTOR_SET);
        descriptorSetsSkybox.cameraDescriptorSet->WriteDescriptorSets(descriptorSets);

        descriptorSets =
        {
         {
             ShaderProgramDescriptorType::CombineImageSampler,
             SKYBOX_BINDING,
             nullptr,
             &skyboxCubeMapDescritptor,
         }
        };
        m_CubeMapShader.lock()->AllocDescriptorSet(&descriptorSetsSkybox.cubeMapDescriptorSet, ENVIRONEMENT_DESCRIPTOR_SET);
        descriptorSetsSkybox.cubeMapDescriptorSet->WriteDescriptorSets(descriptorSets);
    }
   
}
#pragma endregion CreateDescriptorSets