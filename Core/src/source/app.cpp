#include "app.hpp"

#include <iostream>

#include "log.hpp"
#include "resources/resource_manager.hpp"
#include "world/transform.hpp"

using namespace PC_CORE;

App::App()
{
    PC_LOG("App Init")
    windowHandle.Init();
    
    VulkanInterface::Init(&windowHandle);
    ResourceManager::Init();
    renderer.Init(&windowHandle);

    World::world = &world;
    const Entity entity = world.scene.CreateEntity();
    world.scene.AddComponent<Transform>(entity);
}

App::~App()
{
    PC_LOG("App Destroy")
    windowHandle.Destroy();
    renderer.Destroy();
    ResourceManager::Destroy();
    VulkanInterface::Destroy();
    
    delete ComponentRegister::componentRegisterMap;
}

void App::Run()
{
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        renderer.RenderViewPort();
       
    }   
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
