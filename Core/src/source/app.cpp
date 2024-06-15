#include "app.hpp"

#include <iostream>

#include "imgui.h"
#include "log.hpp"
#include "physics/sphere_collider.hpp"
#include "rendering/light.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;


void App::Init()
{
    PC_LOG("App Init")
    windowHandle.Init();
    
    VulkanInterface::Init(&windowHandle);
    ResourceManager::Init();
    renderer.Init(&windowHandle);
    vulkanImgui.Init(windowHandle);
}

void App::Destroy()
{
    PC_LOG("App Destroy")
    renderer.Destroy();
    // Wait the device has been stop by the destroy render func
    vulkanImgui.Destroy();
    ResourceManager::Destroy();
    VulkanInterface::Destroy();
    windowHandle.Destroy();
    
    delete ComponentRegister::componentRegisterMap;
}


void App::Run()
{
    /*
    VulkanViewport vulkanViewport;

    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();
        MoveObject();
        renderer.BeginFrame();
        renderer.RenderViewPort(camera, vulkanViewport, world);
        renderer.SwapBuffers();
    } */  
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


void App::MoveObject()
{
    if (ImGui::Begin("MoveObject"))
    {
        for (Transform* transform : transforms)
        {
            if (transform != nullptr)
            {
                ImGui::PushID(transform->componentHolder.entityID);
                ImGui::Text("entity %d",transform->componentHolder.entityID);
                ImGui::DragFloat3("Position",transform->localPosition.GetPtr(),1.f, -10000.f, 10000.f);
                ImGui::DragFloat4("Rotation",transform->localRotation.imaginary.GetPtr(),1.f, -10000.f, 10000.f);
                ImGui::DragFloat3("Scale",transform->scale.GetPtr(),1.f, -10000.f, 10000.f);
                ImGui::PopID();
            }
        }

        ImGui::End();
    }
}


