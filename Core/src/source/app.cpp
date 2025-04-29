#include "app.hpp"

#include <iostream>

#include "log.hpp"
#include "physics/sphere_collider.hpp"
#include "resources/resource_manager.hpp"

#include "time/core_time.hpp"


using namespace PC_CORE;



void App::Init(const AppCreateInfo& _appCreateInfo)
{
    PC_LOG("App Init")
    // Can init without any depedancies
    window = Window("Para Conquer Editor", _appCreateInfo.appLogoPath.c_str());

    const RenderHardwareInterfaceCreateInfo createInfo =
        {
        .GraphicsAPI = GraphicAPI::VULKAN,
        .window = &window,
        .appName = "Para Conquer Editor",
        };
    
    rhi = Rhi(createInfo);
    ResourceManager::InitPath();
    renderer.Init();
    Time::Init();
    
    world.LoadSkyBox();
}

void App::Destroy()
{

    world.skybox.Destroy();
    world.Destroy();
    renderer.Destroy();
    ResourceManager::Destroy();
    PC_LOG("App Destroy")
}
    
App::App()
{
    instance = this;
}


void App::WorldTick()
{
    //world.sceneGraph.UpdateTransforms(&world.scene);
    
    if (world.begin)
    {
        //physicsWrapper.InitBodies(&world.scene);
        world.Begin();
        world.begin = false;
        world.run = true;
    }
    
    if (world.run)
    {
        //physicsWrapper.UpdatePhysics(PC_CORE::Time::DeltaTime(), &world.scene);
        world.Update();
    }
    //world.sceneGraph.UpdateMatrix(&world.scene);
}

