#include "app.hpp"

#include <iostream>

#include "rendering/log.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

App::App()
{
    PC_LOG("App Init")
    windowHandle.Init();
    ResourceManager::Init();
    
    renderer.Init(windowHandle.window);
}

App::~App()
{
    PC_LOG("App Destroy")
    windowHandle.Destroy();
    ResourceManager::Destroy();
}

void App::Run()
{
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
    }   
}
