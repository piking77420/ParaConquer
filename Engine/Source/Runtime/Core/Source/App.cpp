#include "App.hpp"

#include <Iostream>

#include "Log.hpp"
#include "Physics/SphereCollider.hpp"
#include "Resources/ResourceManager.hpp"
#include "Thread/ThreadUtils.hpp"

#include "Time/CoreTime.hpp"


using namespace PC_CORE;


void App::Init(const AppCreateInfo& _appCreateInfo)
{
    PERF_REGION_SCOPED;
    PC_LOG("App Init")
    // Can init without any depedancies
    MainWindow = Window(_appCreateInfo.appName.data());
    MainWindow.SetIcon(_appCreateInfo.appLogoPath.data());

    const RenderHardwareInterfaceCreateInfo createInfo =
    {
        .GraphicsAPI = GraphicAPI::Vulkan,
        .window = &MainWindow,
        .appName = _appCreateInfo.appName.data(),
        .gpuDebug = _appCreateInfo.enableGpuDebug
    };

    RenderHarwareInteface.Init(createInfo);
    PrimaryCommandBuffer.reset(RenderHarwareInteface.CreateCommandList());
    PrimaryCommandBuffer
        ->SetName("PrimaryCommandBuffer")
        .Build();

    TextureSampler.reset(RenderHarwareInteface.CreateSampler());
    TextureSampler
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetMipmapMode(SamplerMipmapMode::Linear)
        .SetU(SamplerAddressMode::Repeat)
        .SetV(SamplerAddressMode::Repeat)
        .SetW(SamplerAddressMode::Repeat)
        .SetName("TextureSampler")
        .Build();

    DummyTexture.reset(RenderHarwareInteface.CreateTexture());
    DummyTexture
        ->SetTextureType(RhiTexture::Type::Texture2D)
        .SetMemoryUsage(RhiTexture::MemoryUsage::StaticGPU)
        .SetTextureUsage(RhiTexture::TextureUsageFlagBits::All)
        .SetWidth(64)
        .SetHeight(64)
        .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
        .SetName("DummyTexture")
        .Build();

    RHI::ResourceUpdateBranch* branch = RenderHarwareInteface.GetRhiContext().ResourceUpdateBranch();

    std::unique_ptr<uint8_t[]> dummyTextureData = std::make_unique<uint8_t[]>(DummyTexture->GetWidth() * DummyTexture->GetHeight());
    branch->
        TextureUpload2D(*DummyTexture.get(), std::move(dummyTextureData), static_cast<size_t>(DummyTexture->GetWidth() * DummyTexture->GetHeight()), RhiResourceState::FragmentShaderResource);
        

    Time::Init();
}

void App::Destroy()
{
    ResourceManager::Destroy();
    PC_LOG("App Destroy")
}

App::App()
    : Renderer(RenderHarwareInteface)
    , ThreadPool()
{
    Instance = this;
}

void App::WorldTick(double _tick)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    World.Begin();
    World.Update(_tick);
    World.RenderingTick(_tick);
}

void App::RenderFrame()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);
    PC_CORE::RhiSwapChain* swapChain = RenderHarwareInteface.GetRhiContext().rhiSwapChain.get();
    PC_CORE::Window* mainWindow = &MainWindow;
    constexpr std::array<float, 4> Color = {
        0.5f,
        0.5f,
        0.5f,
        0.5f,
    };


    if (swapChain->GetSwapChainImageIndex(mainWindow))
    {
        RenderHarwareInteface.GetRhiContext().ProceedResourceUpdateBranch();

        PrimaryCommandBuffer->BeginRecordCommands();
        {
            PrimaryCommandBuffer->BeginDebugLabel("SwapChain", Color);
            swapChain->BeginSwapChainRenderPass(PrimaryCommandBuffer.get());
            OnRender(PrimaryCommandBuffer.get());
            swapChain->EndSwapChainRenderPass(PrimaryCommandBuffer.get());
            PrimaryCommandBuffer->EndDebugLabel();
        }
        PrimaryCommandBuffer->EndRecordCommands();

        RenderHarwareInteface.GetRhiContext().SendEnqueuCommand(PrimaryCommandBuffer.get(), PC_CORE::GpuPipelineStage::ColorAttachmentOutput);
        swapChain->Present(&MainWindow);
        RenderHarwareInteface.NextFrame();
    }

}

