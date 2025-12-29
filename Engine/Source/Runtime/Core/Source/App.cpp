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

    SecondCommandBuffer.reset(RenderHarwareInteface.CreateCommandList());
    SecondCommandBuffer
        ->SetName("SecondCommandBuffer")
        .Build();

    Time::Init();
}

void App::Destroy()
{
    ResourceManager::Destroy();
    PC_LOG("App Destroy")
}

App::App()
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

        {
            SecondCommandBuffer->BeginRecordCommands();
            SecondCommandBuffer->BeginDebugLabel("SecondCommandBuffer", { 1.f,0.f,1.f, 1.f });
            SecondCommandBuffer->EndDebugLabel();
            SecondCommandBuffer->EndRecordCommands();
            RenderHarwareInteface.GetRhiContext().SendEnqueuCommand(SecondCommandBuffer.get(), PC_CORE::GpuPipelineStage::ColorAttachmentOutput);
        }
       
        PrimaryCommandBuffer->BeginRecordCommands();
        {
            PrimaryCommandBuffer->BeginDebugLabel("SwapChain", Color);
            swapChain->BeginSwapChainRenderPass(PrimaryCommandBuffer.get());
            OnSwapChainRender(PrimaryCommandBuffer.get());
            swapChain->EndSwapChainRenderPass(PrimaryCommandBuffer.get());
            PrimaryCommandBuffer->EndDebugLabel();
        }
        PrimaryCommandBuffer->EndRecordCommands();


        RenderHarwareInteface.GetRhiContext().SendEnqueuCommand(PrimaryCommandBuffer.get(), PC_CORE::GpuPipelineStage::ColorAttachmentOutput);
        swapChain->Present(&MainWindow);
    }

}

