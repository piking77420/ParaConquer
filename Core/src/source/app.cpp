#include "app.hpp"

#include <iostream>

#include "log.hpp"
#include "physics/sphere_collider.hpp"
#include "resources/resource_manager.hpp"

#include "lua/lua.hpp"
#include "time/core_time.hpp"


using namespace PC_CORE;

#define LOGO_PATH_WHITE "Logo/paraconquer_logo_white.png"
#define LOGO_PATH_BLACK "Logo/paraconquer_logo_black.png"


void App::Init()
{
    PC_LOG("App Init")
    // Can init without any depedancies
    window = Window("Para Conquer Editor", LOGO_PATH_BLACK);

    const RenderHardwareInterfaceCreateInfo createInfo =
        {
        .GraphicsAPI = GraphicAPI::VULKAN,
        .window = &window,
        .appName = "Para Conquer Editor",
        };
    
    rhi = Rhi(createInfo);
    renderer.Init();

    // Need other init in order to init
    ResourceManager::InitPath();
    //renderer.InitRenderResources();
    Time::Init();
    
    world.LoadSkyBox();
}

void App::Destroy()
{

    world.skybox.Destroy();
    ResourceManager::Destroy();
    //renderer.Destroy();
    PC_LOG("App Destroy")
}

App::App()
{
    instance = this;
}


void App::Run()
{
    while (!window.ShouldClose())
    {
        window.PoolEvents();
        PC_CORE::Time::UpdateTime();
        //HandleResize();
        //vulkanImgui.NewFrame();
        //MoveObject();
        renderer.BeginDraw(&window);
        renderer.Render();
        renderer.SwapBuffers(&window);
    } 
}

void App::WorldLoop()
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

