#include "Rendering/View.hpp"

#include "Rendering/Renderer.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Time/CoreTime.hpp"
#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;

void View::SetRenderingContextFlag(size_t _flag)
{
    RenderingContext.RenderingContextFlag = _flag;
}

void View::SetCamera(Camera* _camera)
{
    m_Camera = _camera;
}

void View::Update()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);
    UpdateRenderingContext();

    CameraGpu.Time = RenderingContext.Time;
    CameraGpu.Deltatime = RenderingContext.DeltaTime;

    const Tbx::Matrix4x4f view = Tbx::LookAtRH<float>(Tbx::Vector3f::Zero(),
                                                      RenderingContext.LowLevelCamera.Front,
                                                      RenderingContext.LowLevelCamera.Up);

    const Tbx::Matrix4x4f projection = Tbx::PerspectiveMatrixFlipYAxis<float>(RenderingContext.LowLevelCamera.Fov,
                                                                              RenderingContext.LowLevelCamera.Aspect,
                                                                              RenderingContext.LowLevelCamera.Near,
                                                                              RenderingContext.LowLevelCamera.Far);

    CameraGpu.Vp = projection * view;
    CameraGpu.View = view;
    CameraGpu.Proj = projection;
    CameraGpu.ViewInv = CameraGpu.View.Invert();
    CameraGpu.ProjInv = CameraGpu.Proj.Invert();
    CameraGpu.VpInv = CameraGpu.Vp.Invert();
    CameraGpu.CameraNear = RenderingContext.LowLevelCamera.Near;
    CameraGpu.CameraFar = RenderingContext.LowLevelCamera.Far;
    CameraGpu.CameraPos = static_cast<Tbx::Vector3f>(RenderingContext.LowLevelCamera.Position);


    PostProcessGpu.Gamma = 2.2;
    PostProcessGpu.Exposure = 1;
}


void View::Resize(Tbx::Vector2i _viewPortSize)
{
    PC_LOG_VERBOSE("Resize View");
    m_CurrentSize = _viewPortSize;
    m_Renderer->GetRhi().GetRhiContext().WaitIdle(); // this func should be remove when deferred destroy will be implemented

    CreateImages();
    CreateFrameBuffers();
    CreateDescritproSets();
}

void View::UpdateRenderingContext()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    if (m_Camera == nullptr)
        return;

    // Camera
    RenderingContext.LowLevelCamera =
    {
        .Position = m_Camera->Position,
        .Front = static_cast<Tbx::Vector3f>(m_Camera->Front),
        .Up = static_cast<Tbx::Vector3f>(m_Camera->Up),
        .Aspect = m_Camera->GetAspect(),
        .Fov = m_Camera->GetFov(),
        .Near = m_Camera->GetNear(),
        .Far = m_Camera->GetFar(),
        .IsOrthographic = m_Camera->GetProjectionType() == ProjectionType::Orthographic,
    };
    RenderingContext.Time = Time::GetTime();
    RenderingContext.DeltaTime = Time::DeltaTime();

    // Frame Buffer
    RenderingContext.ForwardFrameBuffer = m_FrameBuffers.ForwardFrameBuffer;
    RenderingContext.GbufferFrameBuffer = m_FrameBuffers.GbufferFrameBuffer;
    RenderingContext.FinalImageFrameBuffer = m_FrameBuffers.FinalImageFrameBuffer;

    // Descriptor
    RenderingContext.GeometryDescritproSet = m_DescriptorSets.GeometryPass.get();
    RenderingContext.DefferdLightingGbufferSet = m_DescriptorSets.DefferedPassGbuffers.get();
    RenderingContext.DefferdLightingLightingCameraSet = m_DescriptorSets.DefferedPassCameraLight.get();
    RenderingContext.ForwardDesritptorSet = m_DescriptorSets.ForwardDescriptor.get();
    RenderingContext.ToneMapDescritptorSet = m_DescriptorSets.ToneMap.get();
    RenderingContext.FinalImageDescritptorSet = m_DescriptorSets.FinalViewPort.get();

    RenderingContext.HdrImage = &ForwardTexture.color;
    RenderingContext.RenderingContextSize = {
        static_cast<uint32_t>(m_CurrentSize.x), static_cast<uint32_t>(m_CurrentSize.y)
    };
}

void View::CreateImages()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    Rhi& Rhi = m_Renderer->GetRhi();

    Gbuffers.CreateGBuffers(Rhi, m_CurrentSize);

    // Forward
    {
        RhiTexture::RhiTextureDesciptor createInfo =
        {
            .Width = static_cast<uint32_t>(m_CurrentSize.x),
            .Height = static_cast<uint32_t>(m_CurrentSize.y),
            .Depth = 1,
            .Level = 1,
            .LayerCount = 1,
            .Samples = 1,
            .TextureType = RhiTexture::Type::Texture2D,
            .TextureUsage = static_cast<RhiTexture::TextureUsageFlagBits>(RhiTexture::RenderTarget | RhiTexture::Sampled | RhiTexture::Storage),
            .RhiFormat = RhiFormat::R16G16B16A16Sfloat,
        };

        ForwardTexture.color = Texture2D(Rhi, "View Forward Texture", createInfo, RhiResource::MemoryUsage::Dynamic);
        ForwardTexture.color->Build();

        createInfo.RhiFormat = RhiFormat::D24UnormS8Uint;
        createInfo.TextureUsage = static_cast<RhiTexture::TextureUsageFlagBits>(RhiTexture::RenderTarget);

        ForwardTexture.depth = Texture2D(Rhi, "View DepthTexture", createInfo, RhiResource::MemoryUsage::Dynamic);
        ForwardTexture.depth->Build();

    }


    // Final Image
    {
        RhiTexture::RhiTextureDesciptor createInfo =
        {
            .Width = static_cast<uint32_t>(m_CurrentSize.x),
            .Height = static_cast<uint32_t>(m_CurrentSize.y),
            .Depth = 1,
            .Level = 1,
            .LayerCount = 1,
            .Samples = 1,
            .TextureType = RhiTexture::Type::Texture2D,
            .TextureUsage = static_cast<RhiTexture::TextureUsageFlagBits>(RhiTexture::RenderTarget | RhiTexture::Sampled),
            .RhiFormat = RhiFormat::R8G8B8A8Unorm,
        };

        FinalImage = Texture2D(Rhi, "View Final Image", createInfo, RhiResource::MemoryUsage::Dynamic);
        FinalImage->Build();

        createInfo.Samples = Rhi.GetRhiContext().rhiPhysicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount();
        ResolvedImages = Texture2D(Rhi, "View ResolvedImages", createInfo, RhiResource::MemoryUsage::Dynamic);
        ResolvedImages->Build();

    }
}

void View::CreateFrameBuffers()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    Rhi& Rhi = m_Renderer->GetRhi();


    CreateFrameInfo createFrameBufferInfo =
    {
        .Width = static_cast<uint32_t>(m_CurrentSize.x),
        .Height = static_cast<uint32_t>(m_CurrentSize.y),
    };


    // Gbuffer FrameBuffer
    {
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;

        constexpr size_t DepthCount = 1;
        constexpr size_t LitImageCount = 1;

        attechementDescriptor.resize(static_cast<size_t>(GbufferType::Count) + DepthCount + LitImageCount);

        size_t i = 0;
        for (; i < static_cast<size_t>(GbufferType::Count); i++)
            attechementDescriptor[i].RhiTexture = Gbuffers.gbuffers[i].Get();


        attechementDescriptor[i++].RhiTexture = ForwardTexture.color.Get();
        attechementDescriptor[i++].RhiTexture = ForwardTexture.depth.Get();

        assert(ForwardTexture.depth.Get() == attechementDescriptor[attechementDescriptor.size() - 1].RhiTexture &&
            "Backend expect thaht depth is the last attachement");

        createFrameBufferInfo.Attachements = &attechementDescriptor;
        createFrameBufferInfo.RenderPass = m_Renderer->RenderPasses.DefferedPass.get();

        // HANDLE INPUT ATTACHEMENT
        //m_FrameBuffers.GbufferFrameBuffer.reset(Rhi::CreateFrameBuffer("Gbuffer FrameBuffer", createFrameBufferInfo));
    }

    // Forward FrameBuffer
    {
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
        attechementDescriptor.resize(2); // color + depth;
        attechementDescriptor[0].RhiTexture = ForwardTexture.color.Get();
        attechementDescriptor[1].RhiTexture = ForwardTexture.depth.Get();


        createFrameBufferInfo.Attachements = &attechementDescriptor;
        createFrameBufferInfo.RenderPass = m_Renderer->RenderPasses.ForwardPass.get();

        m_FrameBuffers.ForwardFrameBuffer.reset(Rhi.CreateFrameBuffer("Forward FrameBuffer",createFrameBufferInfo)) ;
    }


    // Final Image
    {
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
        attechementDescriptor.resize(2); // color + depth;

        attechementDescriptor[0].RhiTexture = ResolvedImages.Get();
        attechementDescriptor[1].RhiTexture = FinalImage.Get();

        createFrameBufferInfo.Attachements = &attechementDescriptor;
        createFrameBufferInfo.RenderPass = m_Renderer->RenderPasses.DrawToFinalViewPort.get();

        m_FrameBuffers.FinalImageFrameBuffer.reset(Rhi.CreateFrameBuffer("Final Image FrameBuffer", createFrameBufferInfo));
    }
}

void View::CreateDescritproSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const BufferDescriptor cameraBufferDescritptor
    {
        .buffer = m_Renderer->UniformBuffers.CameraUniformBuffer.Get(),
    };

    const BufferDescriptor lightData
    {
        .buffer = m_Renderer->UniformBuffers.LightBuffer.Get(),
    };

    const ImageSamplerDescriptor skyboxCubeMapDescritptor
    {
        .sampler = m_Renderer->LinearReapeat.Get(),
        .texture = m_Renderer->Cubemap.Lock()->Get(),
        .resourceState = RhiResourceState::ShaderRead
    };

    std::vector<ShaderProgramDescriptorWrite> descriptorWrites;


    {
        /*
        PERF_REGION_SCOPED_NAMED("Create geometryPass DescriptorSets");
        std::shared_ptr<GraphicShader> geometryShaderPass = m_Renderer->GeometryBufferShader.lock();
        descriptorWrites =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                cameraBufferDescritptor,
            }
        };

        if (m_DescriptorSets.GeometryPass != nullptr)
            geometryShaderPass->FreeDescriptorSet(&m_DescriptorSets.GeometryPass);

        geometryShaderPass->CreateDescriptorBinding(&m_DescriptorSets.GeometryPass, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.GeometryPass->SetBindings(descriptorWrites);*/
    }

    // Defferd Lighting Descritptors
    {
        /*
        PERF_REGION_SCOPED_NAMED("Create Defferd DescriptorSets");
        // Gbuffers
        std::shared_ptr<GraphicShader> deferredShader = m_Renderer->DeferedShader.lock();
        descriptorWrites.resize(static_cast<uint8_t>(GbufferType::Count));

        std::array<InputAttachementDescriptor, static_cast<uint8_t>(GbufferType::Count)> inputAttachements;

        for (size_t i = 0; i < static_cast<uint8_t>(GbufferType::Count); i++)
        {
            inputAttachements[i] =
                {
                    .image = Gbuffers.gbuffers[i].Get(),
                    .resourceState = RhiResourceState::ShaderRead
                },

                descriptorWrites[i] =
                {
                    .shaderProgramDescriptorType = ShaderProgramDescriptorType::InputAttachment,
                    .bindingIndex = static_cast<uint32_t>(i),
                    .descriptor = inputAttachements[i]
                };
        }

        if (m_DescriptorSets.DefferedPassGbuffers != nullptr)
            deferredShader->FreeDescriptorSet(&m_DescriptorSets.DefferedPassGbuffers);

        deferredShader->CreateDescriptorBinding(&m_DescriptorSets.DefferedPassGbuffers, GBUFFER_SET);
        m_DescriptorSets.DefferedPassGbuffers->SetBindings(descriptorWrites);


        descriptorWrites.resize(2);
        // Camera And Lighting
        descriptorWrites =
        {
            {
                ShaderProgramDescriptorType::UniformBuffer,
                0,
                lightData,
            }
        };

        if (m_DescriptorSets.DefferedPassCameraLight != nullptr)
            deferredShader->FreeDescriptorSet(&m_DescriptorSets.DefferedPassCameraLight);

        deferredShader->CreateDescriptorBinding(&m_DescriptorSets.DefferedPassCameraLight, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.DefferedPassCameraLight->SetBindings(descriptorWrites);*/
    }

    {
        
        PERF_REGION_SCOPED_NAMED("Create Forward Shader DescriptorSet");
        RhiShaderProgram* forwardShader = m_Renderer->ForwardShader.get();

        descriptorWrites =
        {

            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                cameraBufferDescritptor,
            }/*,
            {
                ShaderProgramDescriptorType::UniformBuffer,
                LIGHTDATA_BINDING,
                lightData,
            },
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                FORWARD_SKYBOX_CUBEMAP,
                skyboxCubeMapDescritptor,
            }*/
        };


        m_DescriptorSets.ForwardDescriptor.reset(forwardShader->CreateDescriptorBinding("View Forward Descriptor"));
        m_DescriptorSets.ForwardDescriptor->SetBindings(descriptorWrites, SCENE_DESCRIPTOR_SET).Build();
    }


    {
        PERF_REGION_SCOPED_NAMED("Create ToneMap DescriptorSet");

        RhiShaderProgram* ToneMappShader = m_Renderer->AcesShader.get();

        struct ImageDescriptor hdrImage
        {
            .texture = ForwardTexture.color.Get(),
            .resourceState = RhiResourceState::ComputeWrite,
        };


        descriptorWrites.resize(1);
        descriptorWrites =
        {

            {
                ShaderProgramDescriptorType::StorageImage,
                0,
                hdrImage,
            },
        };

        m_DescriptorSets.ToneMap.reset(ToneMappShader->CreateDescriptorBinding("Tome map view binging"));
        m_DescriptorSets.ToneMap->SetBindings(descriptorWrites, 0);
    }

    {
        PERF_REGION_SCOPED_NAMED("FinalViewPort DescriptorSet");
        RhiShaderProgram* drawToFinalImage = m_Renderer->DrawTextureScreenQuadShader.get();

        const ImageSamplerDescriptor finalImageDescrotproSet
        {
            .sampler = m_Renderer->LinearReapeat.Get(),
            .texture = ForwardTexture.color.Get(),
            .resourceState = RhiResourceState::ShaderRead
        };

        descriptorWrites.resize(1);
        descriptorWrites =
        {

            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                0,
                finalImageDescrotproSet,
            }
        };

        m_DescriptorSets.FinalViewPort.reset(drawToFinalImage->CreateDescriptorBinding("drawToFinalImage View Binding"));
        m_DescriptorSets.FinalViewPort->SetBindings(descriptorWrites, 0).Build();
    }
}


View::View(Renderer* _renderer, Tbx::Vector2i _viewPortSize) : m_Renderer(_renderer)
{
    Resize(_viewPortSize);
}
