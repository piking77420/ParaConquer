#include "editor.hpp"

#include "asset_browser.hpp"
#include "edit_world_window.hpp"
#include "hierachy.hpp"
#include "inspector.hpp"
#include "profiler.hpp"
#include "scene_button.hpp"
#include "world_view_window.hpp"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "physics/sphere_collider.hpp"
#include "resources/resource_manager.hpp"
#include "rendering/light.hpp"



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
    delete World::world;
    
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

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
    Transform* trans = world.scene.AddComponent<Transform>(entity);
    trans->localPosition = {0.f,4.f,0.f};

    
    StaticMesh* staticMesh =world.scene.AddComponent<StaticMesh>(entity);
    staticMesh->mesh = ResourceManager::Get<Mesh>("sphere.obj");
    staticMesh->material = material;

    const Entity entity3 = world.scene.CreateEntity();
    world.scene.AddComponent<Transform>(entity3);
    auto dir = world.scene.AddComponent<DirLight>(entity3);
    world.scene.GetEntityInternal(entity3)->name = "DirectionalLight";
    dir->intensity = 4.f;

    const Entity plane = world.scene.CreateEntity();
    Transform* ptr = world.scene.AddComponent<Transform>(plane);
    staticMesh = world.scene.AddComponent<StaticMesh>(plane);
    staticMesh->mesh = ResourceManager::Get<Mesh>("cube.obj");
    staticMesh->material = material2;
    //ptr->scale = {20,1,20};
}

void Editor::Run()
{
    while (!windowHandle.ShouldClose())
    {
        World* currentWorld = World::world;
        
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();

        if (currentWorld != nullptr)
            renderer.BeginFrame(*currentWorld);
        
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
        }
        
        if (World::world != nullptr)
            World::world->Run();
        
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
    m_EditorWindows.push_back(new EditWorldWindow(*this,"Scene"));
    m_EditorWindows.push_back(new Profiler(*this,"Profiler"));
    m_EditorWindows.push_back(new Inspector(*this,"Inspector"));
    m_EditorWindows.push_back(new Hierachy(*this,"Hierachy"));
    m_EditorWindows.push_back(new SceneButton(*this,"SceneButton"));
    m_EditorWindows.push_back(new AssetBrowser(*this,"AssetBrowser"));

}

