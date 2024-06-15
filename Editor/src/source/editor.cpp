#include "editor.hpp"

#include "edit_world_window.hpp"
#include "profiler.hpp"
#include "world_view_window.hpp"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "physics/sphere_collider.hpp"
#include "resources/resource_manager.hpp"



using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


void Editor::Init()
{
    App::Init();
    io = &ImGui::GetIO();
    InitEditorWindows();


    World::world = new World;
    InitScene();
}

void Editor::Destroy()
{
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

    delete World::world;
    App::Destroy();
}


void Editor::InitScene()
{
    World& world = *World::world;

    Texture* diamondtexture = ResourceManager::Get<Texture>("diamond_block.jpg");
    Texture* emerauldBlock = ResourceManager::Get<Texture>("viking_room.png");

    Material* material = new Material;
    material->Load({emerauldBlock});
    ResourceManager::Add<Material>("baseMaterial",material);

    Material* material2 = new Material;
    material2->Load({diamondtexture});
    ResourceManager::Add<Material>("baseMaterial2",material2);

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

void Editor::Run()
{
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();
        MoveObject();
        renderer.BeginFrame();
        
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
            
        }
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Render();
        }
        vulkanImgui.EndFrame();
        renderer.SwapBuffers();
    }
    renderer.WaitGPU();
}

void Editor::InitEditorWindows()
{
    EditWorldWindow* edw = new EditWorldWindow(*this);
    m_EditorWindows.push_back(edw);
    m_EditorWindows.push_back(new Profiler(*this));
    m_EditorWindows[0]->name = "EditorWindow";
}

