#include "app.hpp"

#include <iostream>

#include "log.hpp"
#include "physics/sphere_collider.hpp"
#include "rendering/light.hpp"
#include "resources/resource_manager.hpp"

#include "lua/lua.hpp"
#include "time/core_time.hpp"
#include <io/core_io.hpp>


using namespace PC_CORE;


void App::Init()
{
    PC_LOG("App Init")
    // Can init without any depedancies
    instance = this;
    CoreIo::Init();
    window = new Window("Para Conquer Editor");
    renderer.InitRHiAndObject(GraphicAPI::VULKAN, window);

    // Need other init in order to init
    ResourceManager::InitPath();
    renderer.InitRenderResources();
    World::world = &world;
    Time::Init();
    
    world.LoadSkyBox();
}

void App::Destroy()
{
    PC_LOG("App Destroy")

    world.skybox.Destroy();
    ResourceManager::Destroy();


    renderer.Destroy();
    delete window;
    CoreIo::Destroy();
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
    
    if (world.begin)
    {
        physicsWrapper.InitBodies(&world.scene);
        world.Begin();
        world.begin = false;
        world.run = true;
    }
    
    if (world.run)
    {
        physicsWrapper.UpdatePhysics(PC_CORE::Time::DeltaTime(), &world.scene);
        world.Update();
    }
    world.sceneGraph.UpdateMatrix(&world.scene);
}

