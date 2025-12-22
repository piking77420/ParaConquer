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
    Renderer.Init(RenderHarwareInteface);
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


void App::Run()
{
}

void App::WorldTick(double _tick)
{
    PERF_REGION_SCOPED;

    World.Begin();
    World.Update(_tick);
    World.RenderingTick(_tick);
}
