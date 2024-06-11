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
    
    InitScene();
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

void App::InitScene()
{
    Texture* diamondtexture = ResourceManager::Get<Texture>("diamond_block.jpg");
    Texture* emerauldBlock = ResourceManager::Get<Texture>("viking_room.png");

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
