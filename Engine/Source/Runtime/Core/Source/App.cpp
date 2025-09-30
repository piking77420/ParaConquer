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
    window = Window(_appCreateInfo.appName.data());
    window.SetIcon(_appCreateInfo.appLogoPath.data());

    const RenderHardwareInterfaceCreateInfo createInfo =
        {
        .GraphicsAPI = GraphicAPI::Vulkan,
        .window = &window,
        .appName = _appCreateInfo.appName.data(),
        .gpuDebug = _appCreateInfo.enableGpuDebug
        };
    
    rhi = Rhi(createInfo);
    renderer.Init();
    Time::Init();
}

void App::Destroy()
{
    ResourceManager::Destroy();
    PC_LOG("App Destroy")
}
    
App::App()
{
    instance = this;
}


void App::WorldTick(double _tick)
{
    PERF_REGION_SCOPED;

    world.Begin();
    world.Update(_tick);
    world.RenderingTick(_tick);
}

