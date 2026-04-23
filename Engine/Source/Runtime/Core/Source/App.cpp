#include "App.hpp"

#include <Iostream>

#include "Log.hpp"
#include "Physics/SphereCollider.hpp"
#include "Resources/ResourceManager.hpp"
#include "Thread/ThreadUtils.hpp"

#include "Time/CoreTime.hpp"


using namespace PC_CORE;

void App::Destroy()
{
    ResourceManager::Destroy();
    PC_LOG("App Destroy")
}

App::App(const PC_CORE::AppCreateInfo& _AppCreateInfo)
    : MainWindow(_AppCreateInfo.appName.data())
    , RenderHarwareInteface(RenderHardwareInterfaceCreateInfo
    (
        GraphicAPI::Vulkan,
        &MainWindow,
        _AppCreateInfo.appName.data(),
        _AppCreateInfo.enableGpuDebug
    ))
    , Renderer(RenderHarwareInteface)
    , ThreadPool("Main Thread Pool")
{
    Instance = this;

    PERF_REGION_SCOPED;
    PC_LOG("App Init")
        // Can init without any depedancies
    MainWindow.SetIcon(_AppCreateInfo.appLogoPath.data());

    PrimaryCommandBuffer.reset(RenderHarwareInteface.CreateCommandList());
    PrimaryCommandBuffer
        ->SetName("PrimaryCommandBuffer")
        .Build();

    SamplerLinearReapet.reset(RenderHarwareInteface.CreateSampler());
    SamplerLinearReapet
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetMipmapMode(SamplerMipmapMode::Linear)
        .SetU(SamplerAddressMode::Repeat)
        .SetV(SamplerAddressMode::Repeat)
        .SetW(SamplerAddressMode::Repeat)
        .SetName("SamplerLinearReapet")
        .Build();

    SamplerLinearClamp.reset(RenderHarwareInteface.CreateSampler());
    SamplerLinearClamp
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetMipmapMode(SamplerMipmapMode::Linear)
        .SetU(SamplerAddressMode::ClampToBorder)
        .SetV(SamplerAddressMode::ClampToBorder)
        .SetW(SamplerAddressMode::ClampToBorder)
        .SetName("SamplerLinearClamp")
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

    std::unique_ptr<uint8_t[]> dummyTextureData = std::make_unique<uint8_t[]>(DummyTexture->GetWidth() * DummyTexture->GetHeight() * 4);
    branch->
        TextureUpload2D(*DummyTexture.get(), std::move(dummyTextureData), static_cast<size_t>(DummyTexture->GetWidth() * DummyTexture->GetHeight() * 4), RhiResourceState::PixelShaderResource);


    Time::Init();
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

void App::DequeuMainThreadTask()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    while (!m_MainThreadQueue.empty())
    {
        std::function<void()> func;
        {
            std::scoped_lock _(m_MainThreadMutex);
            if (m_MainThreadQueue.empty())
                break;
            func = m_MainThreadQueue.front();
            m_MainThreadQueue.pop();
        }
        func();
    }
}
