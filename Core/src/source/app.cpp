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
    ResourceManager::Init();
    
    renderer.Init(windowHandle.window);

    World::world = &world;

    const Entity entity = world.scene.CreateEntity();
    world.scene.AddComponent<Transform>(entity);
}

App::~App()
{
    PC_LOG("App Destroy")
    windowHandle.Destroy();
    ResourceManager::Destroy();

    delete ComponentRegister::componentRegisterMap;
}

void App::Run()
{
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        renderer.RenderViewPort();
    }   
}
