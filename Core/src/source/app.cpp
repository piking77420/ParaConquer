#include "app.hpp"

#include <iostream>

#include "imgui.h"
#include "log.hpp"
#include "resources/resource_manager.hpp"

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
    transform = world.scene.AddComponent<Transform>(entity);
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
        MoveCam();
        MoveObject();
        ImGui::ShowDemoWindow(&showWindow);
        renderer.RenderViewPort(camera, world);
       
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

void App::MoveCam()
{
    auto io = ImGui::GetIO();
    

    if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonRight))
    {
        Vector2f vec = { -io.MouseDelta.x , -io.MouseDelta.y };
        deltass.AddSample(vec);
        Vector2f avarage = deltass.GetAvarage<Vector2f>();
        yaw += avarage.x;
        pitch += avarage.y;


        constexpr float MaxPitch = 89.f;


        if (pitch > MaxPitch)
            pitch = MaxPitch;
        if (pitch < -MaxPitch)
            pitch = -MaxPitch;

        camera.front.x = std::cos(yaw * Deg2Rad) * std::cos(pitch * Deg2Rad);
        camera.front.y = std::sin(yaw * Deg2Rad) * std::cos(pitch * Deg2Rad);
        camera.front.z = std::sin(pitch * Deg2Rad);
        camera.front = camera.front.Normalize();

    }
    if (ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonRight))
    {
        deltass.Reset();
    }

    const Vector3f right = Vector3f::Cross(camera.front, Vector3f::UnitZ());
    camera.up = Vector3f::Cross(right, camera.front).Normalize();


    Vector3f addVector;
    
    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        addVector += camera.front; 
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        addVector -= camera.front; 
    }

    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        addVector -= right; 
    }
    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
         addVector += right; 
    }

    if (ImGui::IsKeyDown(ImGuiKey_Space))
    {
        addVector +=  camera.up; 
    }
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        addVector -=  camera.up; 
    }

    camera.position += addVector * io.DeltaTime * cameraSpeed;
}

void App::MoveObject()
{
    if (ImGui::Begin("MoveObject"))
    {
        if (transform != nullptr)
        {
            ImGui::DragFloat3("Position",transform->localPosition.GetPtr(),1.f, -10000.f, 10000.f);
            ImGui::DragFloat3("Scale",transform->scale.GetPtr(),1.f, -10000.f, 10000.f);
        }
        ImGui::End();
    }
}
