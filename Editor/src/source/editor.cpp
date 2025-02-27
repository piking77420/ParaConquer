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

    App::Init();


    std::shared_ptr<Material> m1 = ResourceManager::Create<Material>("diamond_block_material");
    m1->m_albedo = ResourceManager::Get<Texture>("diamond_block").get();
    m1->Build();

    std::shared_ptr<Material> m2 = ResourceManager::Create<Material>("emerauld_block_material");

    m2->m_albedo = ResourceManager::Get<Texture>("emerauld_block").get();
    m2->Build();


    InitEditorWindows();
    InitTestScene();
    IMGUIContext.Init(window.GetHandle(), Rhi::GetInstance().GetGraphicsAPI());
    
    renderer.primaryCommandList->RecordFetchCommand([&](CommandList* cmd) {
        IMGUIContext.Render(cmd);
    });

 
}

void Editor::Destroy()
{
    IMGUIContext.Destroy();

    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

    App::Destroy();

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


class TestSystem: public EcsSystem
{
public:

    Signature TransformSignature;

    TestSystem()
    {
        TransformSignature.set(World::GetWorld()->GetComponentTypeBit<Transform>(), true);

        m_SignatureEntitiesSet.insert({ TransformSignature ,{} });


    }

    virtual void Tick(double deltaTime) override
    {
        std::set<EntityId>& entitySet = m_SignatureEntitiesSet[TransformSignature];

        for (auto& ent : entitySet)
        {
            World::GetWorld()->GetComponent<Transform>(ent);


        }

    }

    // Hérité via EcsSystem
    void Begin() override
    {
        return void();
    }

    void RenderingTick(double deltatime) override
    {
        return void();
    }

private:


  
};


void Editor::InitTestScene()
{

    auto testSytem = World::GetWorld()->RegisterSystem<TestSystem>();


    EntityId ent0 = 0;
    ent0 = World::GetWorld()->CreateEntity();
    World::GetWorld()->AddComponent<Transform>(ent0);

    testSytem->Tick(2);
    /*

    EntityId entity_id = world.entityManager.CreateEntity();
    world.entityManager.AddComponent<PC_CORE::Transform>(entity_id);
    world.entityManager.AddComponent<PC_CORE::DirLight>(entity_id);
    world.entityManager.SetEntityName(entity_id, "Directional Light");

    entity_id = world.entityManager.CreateEntity();
    world.entityManager.AddComponent<PC_CORE::Transform>(entity_id);
    Transform& tt = *world.entityManager.GetComponent<Transform>(entity_id);
    
    auto s = world.entityManager.AddComponent<PC_CORE::StaticMesh>(entity_id);
    s->mesh = ResourceManager::Get<Mesh>("sphere").get();
    s->material = ResourceManager::Get<Material>("diamond_block_material").get();

    
    entity_id = world.entityManager.CreateEntity();
    world.entityManager.AddComponent<PC_CORE::Transform>(entity_id);
    Transform& t = *world.entityManager.GetComponent<Transform>(entity_id);

    s = world.entityManager.AddComponent<PC_CORE::StaticMesh>(entity_id);
    s->mesh = ResourceManager::Get<Mesh>("cube").get();
    s->material = ResourceManager::Get<Material>("emerauld_block_material").get();

    t.position = { 0.0f, 2.0f, 1.0f };*/
}

void Editor::DestroyTestScene()
{
    m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;


    //ResourceManager::Delete<Material>("material1");
    //ResourceManager::Delete<Material>("material2");
}

void Editor::Run()
{
    while (!window.ShouldClose())
    {
        coreIo.PoolEvent();
        window.PoolEvents();
        IMGUIContext.NewFrame();
        PC_CORE::Time::UpdateTime();

        UpdateEditorWindows();

        WorldLoop();
      
        if (!renderer.BeginDraw(&window))
        {
            continue;
        }

        for (EditorWindow* editorWindow : m_EditorWindows)
            editorWindow->Render();

 
        renderer.SwapBuffers(&window);
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
