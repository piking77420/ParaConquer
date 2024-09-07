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
#include "serialize/serializer.h"
#include "time/core_time.hpp"


using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


void Editor::Init()
{
    App::Init();
    io = &ImGui::GetIO();
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
        StaticMesh* static_mesh = scene.AddComponent<StaticMesh>(id);
        static_mesh->material = ResourceManager::Get<Material>("baseMaterial");
        static_mesh->mesh = ResourceManager::Get<Mesh>("cube.obj");
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

SerializeTest serialize_test;

void Editor::Run()
{
    while (!windowHandle.ShouldClose())
    {
        const World* currentWorld = World::world;
        windowHandle.PoolEvents();
        PC_CORE::Time::UpdateTime();
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

        if (ImGui::Begin("Test Serialization"))
        {
            ImGui::Text("Test2");
            ImGui::DragInt("caca", &serialize_test.test2.caca, 0.1f);
            ImGui::DragFloat("x", &serialize_test.test2.x, 0.1f);
            ImGui::InputText("string ", serialize_test.test2.name.data(), serialize_test.test2.name.size());

            ImGui::Spacing();
            ImGui::DragFloat("fvalue", &serialize_test.fvalue, 0.1f);
            ImGui::DragInt("value", &serialize_test.value, 0.1f);

            if (ImGui::Button("Serialize"))
            {
                Serializer::Serialize<SerializeTest>(serialize_test, "TestSerialize.meta");
            }
            if (ImGui::Button("Deserialize"))
            {
                Serializer::DeSerialize<SerializeTest>(&serialize_test, "TestSerialize.meta");
            }
            
            ImGui::End();
        }

        if (World::world != nullptr)
        {
            WorldLoop();
            renderer.UpdateWorldBuffers();
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
    m_EditorWindows.push_back(new EditWorldWindow(*this, "Scene"));
    m_EditorWindows.push_back(new Profiler(*this, "Profiler"));
    m_EditorWindows.push_back(new Inspector(*this, "Inspector"));
    m_EditorWindows.push_back(new Hierachy(*this, "Hierachy"));
    m_EditorWindows.push_back(new SceneButton(*this, "SceneButton"));
    m_EditorWindows.push_back(new AssetBrowser(*this, "AssetBrowser"));
}
