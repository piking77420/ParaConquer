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

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


void Editor::Init()
{
    App::Init();
    InitEditorWindows();
    InitMaterial();
    InitTestScene();
}

void Editor::Destroy()
{
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

    App::Destroy();
}


void Editor::InitMaterial()
{
    
    Texture* diamondtexture = ResourceManager::Get<Texture>("diamond_block.jpg");
    Texture* emerauldBlock = ResourceManager::Get<Texture>("viking_room.png");
    Material* material = new Material;
    material->Load({emerauldBlock});
    ResourceManager::Add<Material>("baseMaterial", material);

    Material* material2 = new Material;
    material2->Load({diamondtexture});
    ResourceManager::Add<Material>("baseMaterial2", material2);
    
}

void Editor::InitTestScene()
{
    Scene& scene = world.scene;

    EntityId dirLigthId = scene.CreateEntity("dirLigth");
    scene.AddComponent<DirLight>(dirLigthId);
    scene.AddComponent<Transform>(dirLigthId);

    for (size_t i = 0; i < 20; ++i)
    {
        EntityId id = scene.CreateEntity("test " + std::to_string(i));
        scene.AddComponent<Transform>(id);
        //StaticMesh* static_mesh = scene.AddComponent<StaticMesh>(id);
        //static_mesh->material = ResourceManager::Get<Material>("baseMaterial");
        //static_mesh->mesh = ResourceManager::Get<Mesh>("cube.obj");
    }
    
    
}

void Editor::DestroyTestScene()
{
    physicsWrapper.DestroyBodies(&world.scene);
    world.scene.~Scene();
    world.scene = Scene();
    ResourceManager::Delete<Material>("baseMaterial");
    ResourceManager::Delete<Material>("baseMaterial2");
}

void Editor::Run()
{
    while (!window->ShouldClose())
    {
        const World* currentWorld = World::world;
        PC_CORE::CoreIo::PoolEvent();
        window->Update();
        PC_CORE::Time::UpdateTime();
        
        /*vulkanImgui.NewFrame();

        if (currentWorld != nullptr)
            renderer.BeginFrame(*currentWorld);
            */
        
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
        }

        if (World::world != nullptr)
        {
            WorldLoop();
            //renderer.UpdateWorldBuffers();
        }
        
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Render();
        }
        /*vulkanImgui.EndFrame();
        renderer.SwapBuffers();*/
    }
  // renderer.WaitGPU();
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
