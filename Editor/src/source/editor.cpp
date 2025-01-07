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
    InitEditorWindows();
    InitTestScene();
    PC_CORE::IMGUIContext::Init(window.GetHandle(), renderer.GetGraphicsAPI());


    RotateCube();
}

void Editor::Destroy()
{
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;

    PC_CORE::IMGUIContext::Destroy();
    App::Destroy();
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
    
}

void Editor::InitTestScene()
{
    // TO DO MAKE IT WORK
   /* const ShaderProgram& program = *renderer.forwardShader;
    Material* material1 = ResourceManager::Create<Material>("material1", program);
    material1->albedo = ResourceManager::Get<Texture>("ebony_shield_d.png");

    Material* material2 = ResourceManager::Create<Material>("material2", program);
    material2->albedo = ResourceManager::Get<Texture>("diamond_block.jpg");*/

    const ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo =
        {
            .polygonMode = PolygonMode::Fill,
            .cullModeFlag = CullModeFlagBit::Back,
            .frontFace = FrontFace::CounterClockwise
        },
        .vertexInputBindingDescritions = {Vertex::GetBindingDescrition(0)},
        .vertexAttributeDescriptions = {Vertex::GetAttributeDescriptions(0)}
    };
    
    ShaderInfo shaderInfo =
        {
        .shaderProgramPipelineType = ShaderProgramPipelineType::POINT_GRAPHICS,
        .shaderInfoData = shaderGraphicPointInfo,
        .descriptorInfo =
            {
            .freeDescriptorSet = true,
            .descriptorAllocCount = DESCRIPTOR_ALLOC_HIGH
            }
        };
    
    ProgramShaderCreateInfo createInfo =
        {
        .prograShaderName = "Forward Material Shader",
        .shaderInfo = shaderInfo,
        .renderPass = renderer.forwardRenderPass.GetHandle()
        };
    
    std::vector<ShaderSource*> shaderSource =
        {
        ResourceManager::Get<ShaderSource>("main.vert"),
        ResourceManager::Get<ShaderSource>("main.frag")
        };
    
    Material* material1 = new Material(createInfo, shaderSource);
    ResourceManager::Add<Material>(material1);

    // TO DO CREATE MATERIAL INSTANCE
    MaterialInstance* m1 = ResourceManager::Create<MaterialInstance>("EbonyShield ", material1);
    m1->albedo = ResourceManager::Get<Texture>("ebony_shield_d.png");
    m1->BuildDescriptorSet();

    MaterialInstance* m2 = ResourceManager::Create<MaterialInstance>("Diamond", material1);
    m2->albedo = ResourceManager::Get<Texture>("diamond_block.jpg");
    m2->BuildDescriptorSet();

    
    EntityManager& scene = world.entityManager;
    for (size_t i = 0; i < 10; i++)
    {
        EntityId cubeId = scene.CreateEntity(std::string("Cube") + std::to_string(i));
        
        scene.AddComponent<Transform>(cubeId);
        scene.AddComponent<RigidBody>(cubeId);
        StaticMesh* mesh = scene.AddComponent<StaticMesh>(cubeId);
        if (i == 0)
        {
            mesh->materialInstance = m1;
        }
        else
        {
            mesh->materialInstance = m2;
        }
        
        mesh->mesh = ResourceManager::Get<Mesh>("suzanne.obj");
    }

    
    
}

#define CONST const

void FuncParam(CONST Transform& t, CONST RigidBody& c , int value, float k)
{
    std::cout << World::GetWorld()->entityManager.GetEntityName(t.entityId) << " " << value << " " <<  k << '\n';
}
void Func(CONST Transform& t, CONST RigidBody& c)
{
    std::cout << World::GetWorld()->entityManager.GetEntityName(t.entityId)  << '\n';
}

void Editor::FuncMember(CONST Transform& t, CONST RigidBody& c)
{
    std::cout << World::GetWorld()->entityManager.GetEntityName(t.entityId)  << "Member " << window.GetWindowSize().x << " " <<
        window.GetWindowSize().y <<'\n';

    Editor& r = *this;
}


void Editor::RotateCube()
{
    
    /*
    std::function<void(Transform&, RigidBody& , int , float)> d = FuncParam;
    World::GetWorld()->entityManager.ForEach<Transform, RigidBody>(d, 20, 26.f);
    
    std::function<void(Transform&, RigidBody&)> c = Func;
    World::GetWorld()->entityManager.ForEach<Transform, RigidBody>(c);

    // MEMEBER

    c = std::bind(&Editor::FuncMember, this, std::placeholders::_1, std::placeholders::_2);
    World::GetWorld()->entityManager.ForEach<Transform, RigidBody>(c);
    */
    
}


void Editor::DestroyTestScene()
{
   // physicsWrapper.DestroyBodies(&world.entityRegister);
    //world.scene.~Scene();
    //world.scene = Scene();
    world.entityManager.~EntityManager();
    m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;

    ResourceManager::Delete<Material>("material1");
    ResourceManager::Delete<Material>("material2");
}

void Editor::Run()
{
    while (!window.ShouldClose())
    {
        coreIo.PoolEvent();
        window.PoolEvents();
        PC_CORE::IMGUIContext::NewFrame();
        PC_CORE::Time::UpdateTime();
        
        renderer.BeginFrame();
       
        UpdateEditorWindows();

        if (World::GetWorld())
            WorldLoop();

        for (EditorWindow* editorWindow : m_EditorWindows)
            editorWindow->Render();
        
        
        
        PC_CORE::IMGUIContext::Render(renderer.GetCommandSwapChainBuffer());
        renderer.EndRender();
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
