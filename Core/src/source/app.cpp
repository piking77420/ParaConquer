#include "app.hpp"

#include <iostream>

#include "imgui.h"
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
    vulkanImgui.Init(windowHandle);

    World::world = &world;
    const Entity entity = world.scene.CreateEntity();
    world.scene.AddComponent<Transform>(entity);
}

App::~App()
{
    PC_LOG("App Destroy")
    renderer.Destroy();
    // Wait tha the device has been stop by the destroy render func
    vulkanImgui.Destroy();
    ResourceManager::Destroy();
    VulkanInterface::Destroy();
    windowHandle.Destroy();
    
    delete ComponentRegister::componentRegisterMap;
}

void App::Run()
{
    bool showWindow = true;
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();

        ImGui::ShowDemoWindow(&showWindow);
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
