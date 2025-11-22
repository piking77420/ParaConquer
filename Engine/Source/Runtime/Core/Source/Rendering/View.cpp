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
    Rhi::GetRhiContext()->WaitIdle();

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
    RenderingContext.GeometryDescritproSet = m_DescriptorSets.GeometryPass;
    RenderingContext.DefferdLightingGbufferSet = m_DescriptorSets.DefferedPassGbuffers;
    RenderingContext.DefferdLightingLightingCameraSet = m_DescriptorSets.DefferedPassCameraLight;
    RenderingContext.ForwardDesritptorSet = m_DescriptorSets.ForwardDescriptor;
    RenderingContext.ToneMapDescritptorSet = m_DescriptorSets.ToneMap;
    RenderingContext.FinalImageDescritptorSet = m_DescriptorSets.FinalViewPort;

    RenderingContext.HdrImage = &ForwardTexture.color;
    RenderingContext.RenderingContextSize = {
        static_cast<uint32_t>(m_CurrentSize.x), static_cast<uint32_t>(m_CurrentSize.y)
    };
}

void View::CreateImages()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    Gbuffers.CreateGBuffers(m_CurrentSize);

    // Forward
    {
        CreateImageInfo createInfo =
        {
            .Width = m_CurrentSize.x,
            .Height = m_CurrentSize.y,
            .Depth = 1,
            .LayerCount = 1,
            .MipsLevels = 1,
            .TextureType = TextureType::Texture2D,
            .Format = RhiFormat::R16G16B16A16Sfloat,
            .Channel = Channel::Rgba,
            .TextureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled | TextureUsage::Storage,
            .MemoryVisibility = MemoryLocalisation::GpuOnly,
            .Samples = 1,
            .GenerateMipMap = false,
            .Datas = {}
        };

        ForwardTexture.color = Texture2D(createInfo);

        createInfo.Format = RhiFormat::D32Sfloat;
        createInfo.Channel = Channel::Grey;
        createInfo.TextureUsage = TextureUsage::Depth;

        ForwardTexture.depth = Texture2D(createInfo);
    }


    // Final Image
    {
        CreateImageInfo createInfo =
        {
            .Width = m_CurrentSize.x,
            .Height = m_CurrentSize.y,
            .Depth = 1,
            .LayerCount = 1,
            .MipsLevels = 1,
            .TextureType = TextureType::Texture2D,
            .Format = RhiFormat::R8G8B8A8Unorm,
            .Channel = Channel::Rgba,
            .TextureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled,
            .MemoryVisibility = MemoryLocalisation::GpuOnly,
            .Samples = 1,
            .GenerateMipMap = false,
            .Datas = {}
        };

        FinalImage = Texture2D(createInfo);
        createInfo.Samples = Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount();
        createInfo.TextureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled;
        ResolvedImages = Texture2D(createInfo);
    }
}

void View::CreateFrameBuffers()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    CreateFrameInfo createFrameBufferInfo =
    {
        .width = static_cast<uint32_t>(m_CurrentSize.x),
        .height = static_cast<uint32_t>(m_CurrentSize.y),
    };


    // Gbuffer FrameBuffer
    {
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;

        constexpr size_t DepthCount = 1;
        constexpr size_t LitImageCount = 1;

        attechementDescriptor.resize(static_cast<size_t>(GbufferType::Count) + DepthCount + LitImageCount);

        size_t i = 0;
        for (; i < static_cast<size_t>(GbufferType::Count); i++)
            attechementDescriptor[i].texture = &Gbuffers.gbuffers[i];


        attechementDescriptor[i++].texture = &ForwardTexture.color;
        attechementDescriptor[i++].texture = &ForwardTexture.depth;

        assert(&ForwardTexture.depth == attechementDescriptor[attechementDescriptor.size() - 1].texture &&
            "Backend expect thaht depth is the last attachement");

        createFrameBufferInfo.attachements = &attechementDescriptor;
        createFrameBufferInfo.renderPass = m_Renderer->RenderPasses.DefferedPass.get();


        m_FrameBuffers.GbufferFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
    }

    // Forward FrameBuffer
    {
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
        attechementDescriptor.resize(2); // color + depth;
        attechementDescriptor[0].texture = &ForwardTexture.color;
        attechementDescriptor[1].texture = &ForwardTexture.depth;


        createFrameBufferInfo.attachements = &attechementDescriptor;
        createFrameBufferInfo.renderPass = m_Renderer->RenderPasses.ForwardPass.get();

        m_FrameBuffers.ForwardFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
    }


    // Final Image
    {
        std::vector<FrameBufferAttachementDesriptor> attechementDescriptor;
        attechementDescriptor.resize(2); // color + depth;

        attechementDescriptor[0].texture = &ResolvedImages;
        attechementDescriptor[1].texture = &FinalImage;

        createFrameBufferInfo.attachements = &attechementDescriptor;
        createFrameBufferInfo.renderPass = m_Renderer->RenderPasses.DrawToFinalViewPort.get();

        m_FrameBuffers.FinalImageFrameBuffer = Rhi::CreateFrameBuffer(createFrameBufferInfo);
    }
}

void View::CreateDescritproSets()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rendering);

    const UniformBufferDescriptor cameraBufferDescritptor
    {
        .buffer = &m_Renderer->UniformBuffers.CameraUniformBuffer,
    };

    const UniformBufferDescriptor lightData
    {
        .buffer = &m_Renderer->UniformBuffers.DynamicGpuLightUniformBuffer,
    };

    const ImageSamplerDescriptor skyboxCubeMapDescritptor
    {
        .sampler = &m_Renderer->LinearReapeat,
        .texture = m_Renderer->Cubemap.lock().get(),
        .imageState = ImageState::ShaderReadOptimal
    };

    std::vector<ShaderProgramDescriptorWrite> descriptorWrites;


    {
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

        geometryShaderPass->AllocDescriptorSet(&m_DescriptorSets.GeometryPass, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.GeometryPass->WriteDescriptorSets(descriptorWrites);
    }

    // Defferd Lighting Descritptors
    {
        PERF_REGION_SCOPED_NAMED("Create Defferd DescriptorSets");
        // Gbuffers
        std::shared_ptr<GraphicShader> deferredShader = m_Renderer->DeferedShader.lock();
        descriptorWrites.resize(static_cast<uint8_t>(GbufferType::Count));

        std::array<InputAttachementDescriptor, static_cast<uint8_t>(GbufferType::Count)> inputAttachements;

        for (size_t i = 0; i < static_cast<uint8_t>(GbufferType::Count); i++)
        {
            inputAttachements[i] =
                {
                    .image = &Gbuffers.gbuffers[i],
                    .imageState = ImageState::ShaderReadOptimal
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

        deferredShader->AllocDescriptorSet(&m_DescriptorSets.DefferedPassGbuffers, GBUFFER_SET);
        m_DescriptorSets.DefferedPassGbuffers->WriteDescriptorSets(descriptorWrites);


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

        deferredShader->AllocDescriptorSet(&m_DescriptorSets.DefferedPassCameraLight, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.DefferedPassCameraLight->WriteDescriptorSets(descriptorWrites);
    }

    {
        /*
        PERF_REGION_SCOPED_NAMED("Create Forward Shader DescriptorSet");
        std::shared_ptr<GraphicShader> m_ForwardShader = m_Renderer->m_ForwardShader.lock();

        descriptorWrites.resize(3);

        descriptorWrites =
        {

            {
                ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                cameraBufferDescritptor,
            },
            {
                ShaderProgramDescriptorType::UniformBuffer,
                LIGHTDATA_BINDING,
                lightData,
            },
            {
                ShaderProgramDescriptorType::CombinedImageSampler,
                FORWARD_SKYBOX_CUBEMAP,
                skyboxCubeMapDescritptor,
            }
        };


        if (m_DescriptorSets.forwardDescriptor != nullptr)
            m_ForwardShader->FreeDescriptorSet(&m_DescriptorSets.forwardDescriptor);

        m_ForwardShader->AllocDescriptorSet(&m_DescriptorSets.forwardDescriptor, SCENE_DESCRIPTOR_SET);
        m_DescriptorSets.forwardDescriptor->WriteDescriptorSets(descriptorWrites);*/
    }


    {
        PERF_REGION_SCOPED_NAMED("Create ToneMap DescriptorSet");

        std::shared_ptr<ComputeShader> m_ToneMapp = m_Renderer->AcesShader.lock();

        struct ImageDescriptor hdrImage
        {
            .texture = &ForwardTexture.color,
            .imageState = ImageState::General,
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

        if (m_DescriptorSets.ToneMap != nullptr)
            m_ToneMapp->FreeDescriptorSet(&m_DescriptorSets.ToneMap);

        m_ToneMapp->AllocDescriptorSet(&m_DescriptorSets.ToneMap, 0);
        m_DescriptorSets.ToneMap->WriteDescriptorSets(descriptorWrites);
    }

    {
        PERF_REGION_SCOPED_NAMED("FinalViewPort DescriptorSet");
        std::shared_ptr<GraphicShader> drawToFinalImage = m_Renderer->DrawTextureScreenQuadShader.lock();

        const ImageSamplerDescriptor finalImageDescrotproSet
        {
            .sampler = &m_Renderer->LinearReapeat,
            .texture = &ForwardTexture.color,
            .imageState = ImageState::General
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

        if (m_DescriptorSets.FinalViewPort != nullptr)
            drawToFinalImage->FreeDescriptorSet(&m_DescriptorSets.FinalViewPort);

        drawToFinalImage->AllocDescriptorSet(&m_DescriptorSets.FinalViewPort, 0);
        m_DescriptorSets.FinalViewPort->WriteDescriptorSets(descriptorWrites);
    }
}


View::View(Renderer* _renderer, Tbx::Vector2i _viewPortSize) : m_Renderer(_renderer)
{
    Resize(_viewPortSize);
}
