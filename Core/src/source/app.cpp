#include "app.hpp"

#include <iostream>

#include "imgui.h"
#include "log.hpp"
#include "physics/sphere_collider.hpp"
#include "rendering/light.hpp"
#include "resources/resource_manager.hpp"

#include "lua/lua.hpp"
#include "time/core_time.hpp"


using namespace PC_CORE;


void App::Init()
{
    PC_LOG("App Init")
    instance = this;
    World::world = &world;
   
    windowHandle.Init();
    VulkanInterface::Init(&windowHandle);
    ResourceManager::Init();
    renderer.Init(&windowHandle);
    vulkanImgui.Init(windowHandle);
    InitEcs();

    world.LoadSkyBox();
}

void App::Destroy()
{
    PC_LOG("App Destroy")
    // TODO only use Destructor
    world.skybox.Destroy();
    renderer.Destroy();
    // Wait the device has been stop by the destroy render func
    vulkanImgui.Destroy();
    ResourceManager::Destroy();
    VulkanInterface::Destroy();
    windowHandle.Destroy();
}


void App::Run()
{
    /*
    VulkanViewport vulkanViewport;

    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        PC_CORE::Time::UpdateTime();
        HandleResize();
        vulkanImgui.NewFrame();
        MoveObject();
        renderer.BeginFrame();
        renderer.RenderViewPort(camera, vulkanViewport, world);
        renderer.SwapBuffers();
    } */  
}

void App::WorldLoop()
{
    world.sceneGraph.UpdateTransforms(&world.scene);
    if (world.run && world.begin == false)
    {
        physicsWrapper.InitPhysicBody(&world.scene);
        world.Begin();
        world.run = true;
    }
    
    if (world.run)
    {
        physicsWrapper.UpdatePhysics(PC_CORE::Time::GetTime(), &world.scene);
        world.Update();
    }
    
    world.sceneGraph.UpdateMatrix(&world.scene);
}

void App::HandleResize()
{
    if (windowHandle.onResize)
    {
        windowHandle.OnResize();
        renderer.RecreateSwapChain(&windowHandle);
        windowHandle.onResize = false;
    }
}

void App::InitEcs()
{
   
}

