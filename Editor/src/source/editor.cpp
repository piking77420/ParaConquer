#include <glslang/Include/glslang_c_interface.h>

#include "editor.hpp"

#include "asset_browser.hpp"
#include "edit_world_window.hpp"
#include "hierachy.hpp"
#include "inspector.hpp"
#include "profiler.hpp"
#include "scene_button.hpp"
#include "world_view_window.hpp"
#include "time/core_time.hpp"
#include <resources/resource_manager.hpp>
#include "rendering/light.hpp"
#include "io/core_io.hpp"
#include "io/imgui_context.h"
#include "physics/rigid_body.hpp"
#include "rendering/material.hpp"
#include "resources/shader_source.hpp"
#include "world/static_mesh.hpp"
#include "serialize/serializer.h"
#include <thread>

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


void Editor::InitThridPartLib()
{
    glslang_initialize_process();

}

void Editor::UnInitThridPartLib()
{
    glslang_finalize_process();
}

void Editor::CompileShader()
{
    std::shared_ptr<ShaderSource> vertex = ResourceManager::Create<ShaderSource>("assets/shaders/main.vert");
    std::shared_ptr<ShaderSource> frag = ResourceManager::Create<ShaderSource>("assets/shaders/main.frag");

    vertex->CompileToSpriv();
    frag->CompileToSpriv();

    vertex = ResourceManager::Create<ShaderSource>("assets/shaders/draw_texture_screen_quad.vert");
    frag = ResourceManager::Create<ShaderSource>("assets/shaders/draw_texture_screen_quad.frag");

    
    vertex->CompileToSpriv();
    frag->CompileToSpriv();
}

void Editor::Init()
{
    InitThridPartLib();
    CompileShader();

    gameApp.Init();

    
    std::shared_ptr<Material> m1 = ResourceManager::Create<Material>("diamond_block_material.mat");
    m1->m_albedo = ResourceManager::Get<Texture>("diamond_block.jpg");
    m1->Build();

    std::shared_ptr<Material> m2 = ResourceManager::Create<Material>("emerauld_block_material.mat");

    m2->m_albedo = ResourceManager::Get<Texture>("emerauld_block.png");
    m2->Build();


    InitEditorWindows();
    InitTestScene();
    IMGUIContext.Init(gameApp.window.GetHandle(), Rhi::GetInstance().GetGraphicsAPI());
    
    gameApp.renderer.primaryCommandList->RecordFetchCommand([&](CommandList* cmd) {
        IMGUIContext.Render(cmd);
    });


    //Tbx::Vector3f y;
    //Serializer::DeSerialize(&y, "TestSerilize.ser");

}

void Editor::Destroy()
{
    IMGUIContext.Destroy();

    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

    gameApp.Destroy();

    UnInitThridPartLib();
}


void Editor::UpdateEditorWindows()
{
    
    dockSpace.BeginDockSpace();
    ShaderRecompileList();
    for (EditorWindow* editorWindow : m_EditorWindows)
    {
        editorWindow->Begin();
        editorWindow->Update();
        editorWindow->End();
    }
    dockSpace.EndDockSpace();
}

void Editor::ShaderRecompileList()
{
    /*
    static bool open = true;
    ImGui::Begin("Shader Recompile List");

    
    auto lamba = [&](const ShaderProgram* shaderProgram) -> void
    {
      if (ImGui::Button(shaderProgram->name.c_str()))
      {
          PC_LOG("Recompile Shader Program");
          //shaderProgram->Reload();
      }
        
    };
    
    ResourceManager::ForEach<ShaderProgram>(lamba);
    
    ImGui::End();
    */
}


REFLECT(std::vector<Transform>)

void Editor::InitTestScene()
{
    EntityId dirLight = World::GetWorld()->CreateEntity();
    World::GetWorld()->AddComponent<DirLight>(dirLight);
    World::GetWorld()->AddComponent<Transform>(dirLight);

    EntityId cube = World::GetWorld()->CreateEntity();
    World::GetWorld()->AddComponent<Transform>(cube);
    World::GetWorld()->AddComponent<StaticMesh>(cube);


    EntityId sphere = World::GetWorld()->CreateEntity();
    World::GetWorld()->AddComponent<Transform>(sphere);
    World::GetWorld()->AddComponent<StaticMesh>(sphere);
    Transform* t = &World::GetWorld()->GetComponent<Transform>(sphere);
    t->position = { 0.0f, 2.0f, 1.0f };

    StaticMesh* mesh = &World::GetWorld()->GetComponent<StaticMesh>(cube);
    mesh->mesh = ResourceManager::Get<Mesh>("cube.obj");
    mesh->material = ResourceManager::Get<Material>("emerauld_block_material.mat");

    StaticMesh* mesh2 = &World::GetWorld()->GetComponent<StaticMesh>(sphere);
    mesh2->mesh = ResourceManager::Get<Mesh>("sphere.obj");
    mesh2->material = ResourceManager::Get<Material>("diamond_block_material.mat");

    Serializer::Serialize(*mesh, "TestSerilize.ser");
    StaticMesh srMesj;
    Serializer::DeSerialize(&srMesj, "TestSerilize.ser");
    *mesh = srMesj;
         
    //StaticMesh sermesh;
    //Serializer::DeSerialize(&sermesh, "TestSerilize.ser");
}

void Editor::DestroyTestScene()
{
    m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;


    //ResourceManager::Delete<Material>("material1");
    //ResourceManager::Delete<Material>("material2");
}

void Editor::Run(bool* _appShouldClose)
{
        while (!gameApp.window.ShouldClose())
        {
            gameApp.coreIo.PoolEvent();
            gameApp.window.PoolEvents();
            IMGUIContext.NewFrame();
            PC_CORE::Time::UpdateTime();

            UpdateEditorWindows();

            gameApp.WorldTick();
      
            if (!gameApp.renderer.BeginDraw(&gameApp.window))
            {
                continue;
            }

            for (EditorWindow* editorWindow : m_EditorWindows)
                editorWindow->Render();

 
            gameApp.renderer.SwapBuffers(&gameApp.window);
        }

        Rhi::GetRhiContext()->WaitIdle();
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
