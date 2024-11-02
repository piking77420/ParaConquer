#include "editor.hpp"

#include "asset_browser.hpp"
#include "edit_world_window.hpp"
#include "hierachy.hpp"
#include "inspector.hpp"
#include "profiler.hpp"
#include "scene_button.hpp"
#include "world_view_window.hpp"
#include "resources/material.hpp"
#include "time/core_time.hpp"
#include <resources/resource_manager.hpp>
#include "rendering/light.hpp"
#include "io/core_io.hpp"
#include "io/imgui_context.h"
#include "physics/rigid_body.hpp"
#include "world/static_mesh.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


void Editor::Init()
{
    App::Init();
    World::currentWorld = &world;
    InitEditorWindows();
    InitMaterial();
    InitTestScene();
    PC_CORE::IMGUIContext::Init(window->GetHandle());
}

void Editor::Destroy()
{
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

    PC_CORE::IMGUIContext::Destroy();
    App::Destroy();
}


void Editor::InitMaterial()
{
    /*
    Texture* diamondtexture = ResourceManager::Get<Texture>("diamond_block.jpg");
    Texture* emerauldBlock = ResourceManager::Get<Texture>("viking_room.png");
    Material* material = new Material;
    material->Load({emerauldBlock});
    ResourceManager::Add<Material>("baseMaterial", material);

    Material* material2 = new Material;
    material2->Load({diamondtexture});
    ResourceManager::Add<Material>("baseMaterial2", material2);*/
    
}

void Editor::RotateCube()
{
    
    if (!world.run)
        return;

    for (auto& entity : world.scene.m_Entities)
    {
        if (entity.ecsId == INVALID_ENTITY_ID)
            continue;

        Transform* transform = world.scene.GetComponent<Transform>(&entity);
        float time = static_cast<float>(Time::GetTime());
        float x = std::cos(time);
        float y = std::sin(time);
        float z = std::cos(time);
        transform->rotation = Tbx::Quaternionf::FromEuleur({x,y,z});
    }
   
}

void Editor::InitTestScene()
{

    Scene& scene = world.scene;
    for (size_t i = 0; i < 2; i++)
    {
        Entity* cube = scene.CreateEntity("cube " + std::to_string(i));
        scene.AddComponent<Transform>(cube);
        scene.AddComponent<RigidBody>(cube);
        StaticMesh* mesh = scene.AddComponent<StaticMesh>(cube);
        mesh->mesh = ResourceManager::Get<Mesh>("rounded_cube.obj");
    }
    
}

void Editor::DestroyTestScene()
{
    physicsWrapper.DestroyBodies(&world.scene);
    world.scene.~Scene();
    world.scene = Scene();
    m_Selected = nullptr;

    
    //ResourceManager::Delete<Material>("baseMaterial");
    //ResourceManager::Delete<Material>("baseMaterial2");
}

void Editor::Run()
{
    while (!window->ShouldClose())
    {
        PC_CORE::CoreIo::PoolEvent();
        window->PoolEvents();
        PC_CORE::IMGUIContext::NewFrame();
        PC_CORE::Time::UpdateTime();
        
        renderer.BeginFrame();
       
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
        }

        RotateCube();
        if (World::currentWorld != nullptr)
        {
            WorldLoop();
        }
        
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Render();
        }
        PC_CORE::IMGUIContext::Render(renderer.GetCommandSwapChainBuffer());
        renderer.SwapBuffers();
    }

    renderer.WaitDevice();
}

void Editor::InitEditorWindows()
{
    m_EditorWindows.push_back(new EditWorldWindow(*this, "Scene"));
    m_EditorWindows.push_back(new Profiler(*this, "Profiler"));
    m_EditorWindows.push_back(new Inspector(*this, "Inspector"));
    m_EditorWindows.push_back(new Hierachy(*this, "Hierachy"));
    m_EditorWindows.push_back(new SceneButton(*this, "SceneButton"));
    m_EditorWindows.push_back(new AssetBrowser(*this, "AssetBrowser"));
}
