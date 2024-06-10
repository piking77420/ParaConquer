#include "app.hpp"

#include <iostream>

#include "imgui.h"
#include "log.hpp"
#include "physics/sphere_collider.hpp"
#include "rendering/light.hpp"
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
    
   InitScene();
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
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();
        MoveCam();
        MoveObject();
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
    const auto io = ImGui::GetIO();


    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        deltass.Reset();
    }

    
    if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonRight))
    {
        Vector2f vec = { -io.MouseDelta.x , -io.MouseDelta.y };
        deltass.AddSample(vec);
        const Vector2f avarage = deltass.GetAvarage<Vector2f>();
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

void App::InitScene()
{
    Texture* diamondtexture = ResourceManager::CreateAndLoad<Texture>("assets/textures/diamond_block.jpg");
    Texture* emerauldBlock = ResourceManager::CreateAndLoad<Texture>("assets/textures/viking_room.png");

    Material* material = new Material;
    material->Load({emerauldBlock});
    ResourceManager::Add<Material>("baseMaterial",material);

    Material* material2 = new Material;
    material2->Load({diamondtexture});
    ResourceManager::Add<Material>("baseMaterial2",material2);

    World::world = &world;
    const Entity entity = world.scene.CreateEntity();
    transforms.push_back(world.scene.AddComponent<Transform>(entity));
    StaticMesh* staticMesh =world.scene.AddComponent<StaticMesh>(entity);
    staticMesh->mesh = ResourceManager::Get<Mesh>("cube.obj");
    staticMesh->material = material;

    const Entity entity2 = world.scene.CreateEntity();
    transforms.push_back(world.scene.AddComponent<Transform>(entity2));
    StaticMesh* staticMesh2 = world.scene.AddComponent<StaticMesh>(entity2);
    staticMesh2->mesh = ResourceManager::Get<Mesh>("viking_room.obj");
    staticMesh2->material = material2;
    world.scene.AddComponent<SphereCollider>(entity2);

    const Entity entity3 = world.scene.CreateEntity();
    transforms.push_back(world.scene.AddComponent<Transform>(entity3));
    dirLight = world.scene.AddComponent<DirLight>(entity3);
    
}
