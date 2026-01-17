#include <Chrono>
#include <Iostream>
#include <thread>


#include <imgui_internal.h>
#include <PerfRegion.hpp>


#include "Editor.hpp"
#include "Resources/ResourceManager.hpp"


#include "EditWorldWindow.hpp"
#include "Hierachy.hpp"
#include "Inspector.hpp"
#include "ResourceBrowserWindow.hpp"
#include "SceneButton.hpp"
#include "Time/CoreTime.hpp"
#include "WorldViewWindow.hpp"
#include <Resources/ResourceManager.hpp>

#include "EditorFiles.hpp"
#include "Io/CoreIo.hpp"
#include "Io/ImguiContext.h"
#include "Physics/RigidBody.hpp"
#include "ProjectMaker.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderSystem.hpp"
#include "Resources/ShaderSource.hpp"
#include "Serialize/JsonSerializer.hpp"
#include "Serialize/Serializer.h"
#include "SystemDialogue.hpp"
#include "World/StaticMeshComponent.hpp"
#include "Thread/ThreadUtils.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


Editor::Editor()
{
    PROFILER_NOOP;

    if (instance != nullptr)
    {
        PC_LOGERROR("Editor instance is not nullptr");
        exit(-1);
    }
    instance = this;
}

Editor::~Editor()
{
    SaveInitFiles();
    instance = nullptr;
}


void Editor::LoadFromInitFiles()
{
    ProjectFile projectFile;
    EditorIniFile editorIniFile;
    if (std::filesystem::exists(EditorIniFileName)) // if editor.ini exist
    {
        JsonSerializer s;
        s.OpenFile(std::string(EditorIniFileName), Serializer::SerializeOperation::DeSerialize);
        s.DeSerialize<EditorIniFile>(&editorIniFile); // copy it 

        if (std::filesystem::exists(editorIniFile.projectPath)) // if editor.ini is valid
        {
            JsonSerializer s2;
            s2.OpenFile(editorIniFile.projectPath + "/" + std::string(ProjectFileName),
                        Serializer::SerializeOperation::DeSerialize);
            s2.DeSerialize<ProjectFile>(&projectFile); // copy project
            s2.CloseFile();
        }
        else
        {
            const auto sw = std::wstring(editorIniFile.projectPath.begin(), editorIniFile.projectPath.end());
            projectFile = ProjectMaker::CreateBaseProject(sw.c_str());
        }
        s.CloseFile();
    }
    else
    {
        std::wstring projectInit;
        while (projectInit.empty())
        {
            projectInit = SystemDialogue::Instance().SeletecFolder(L"Select your project folder");
        }

        assert(!projectInit.empty() && "Something went wrong");

        if (!std::filesystem::exists(projectInit + std::wstring(ProjectFileName.begin(), ProjectFileName.end())))
        {
            projectFile = ProjectMaker::CreateBaseProject(projectInit.c_str());
        }

        editorIniFile.projectPath = std::string(projectInit.begin(), projectInit.end());
    }

    editorData.projectPath = editorIniFile.projectPath;
    editorData.projectData = ProjectData(projectFile);
}

void Editor::SaveInitFiles()
{
    if (editorData.projectPath.empty())
        return;

    EditorIniFile editorIniFile;
    editorIniFile.projectPath = editorData.projectPath.generic_string();

    JsonSerializer s;
    s.OpenFile(std::string(EditorIniFileName), Serializer::SerializeOperation::Serialize);
    s.Serialize<EditorIniFile>(editorIniFile);
    s.CloseFile();
}

void Editor::CompileShader()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);

    PC_LOG("CompileShader...")

	{
        auto task = []()->void {
            ResourceManager::Create<ShaderSource>("DrawQuadTriangle.vs.hlsl",
                EDITOR_RESOURCE_PATH "/Shaders/DrawQuadTriangle.vs.hlsl");
            };

		m_FuturInits.emplace_back(ThreadPool.Enqueue(task));
        
		m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
			ResourceManager::Create<ShaderSource>("SampleSingleTexture.ps.hlsl",
				EDITOR_RESOURCE_PATH
				"/Shaders/SampleSingleTexture.ps.hlsl");
			}));
	}

	{

        
		m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
			ResourceManager::Create<ShaderSource>("Forward.vs.hlsl",
				EDITOR_RESOURCE_PATH
				"/Shaders/Forward/Forward.vs.hlsl");
			}));

		m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
			ResourceManager::Create<ShaderSource>("Forward.ps.hlsl",
				EDITOR_RESOURCE_PATH
				"/Shaders/Forward/Forward.ps.hlsl");
			}));
	}
    /*
    // sprite
    {
        auto spriteVert = ResourceManager::Create<ShaderSource>("DrawSprite.vs.hlsl",
                                                                EDITOR_RESOURCE_PATH
                                                                "/Shaders/DrawSprite/DrawSprite.vs.hlsl");
        auto spriteFrag = ResourceManager::Create<ShaderSource>("DrawSprite.ps.hlsl",
                                                                EDITOR_RESOURCE_PATH
                                                                "/Shaders/DrawSprite/DrawSprite.ps.hlsl");
    }

    // geometry buffer
    {
        auto geometryVert = ResourceManager::Create<ShaderSource>("Geometry.vs.hlsl",
                                                                  EDITOR_RESOURCE_PATH
                                                                  "/Shaders/Geometry/Geometry.vs.hlsl");

        auto geometryFrag = ResourceManager::Create<ShaderSource>("Geometry.ps.hlsl",
                                                                  EDITOR_RESOURCE_PATH
                                                                  "/Shaders/Geometry/Geometry.ps.hlsl");
    }

    // deferred
    {
        auto deferredFrag = ResourceManager::Create<ShaderSource>("Deferred.ps.hlsl",
                                                                  EDITOR_RESOURCE_PATH
                                                                  "/Shaders/Deferred/Deferred.ps.hlsl");
    }

    {
        // DebugDraw
        auto debugDrawVert = ResourceManager::Create<ShaderSource>("DebugDraw.vs.hlsl",
                                                                   EDITOR_RESOURCE_PATH
                                                                   "/Shaders/DebugDraw/DebugDraw.vs.hlsl");

        auto debugDrawFrag = ResourceManager::Create<ShaderSource>("DebugDraw.ps.hlsl",
                                                                   EDITOR_RESOURCE_PATH
                                                                   "/Shaders/DebugDraw/DebugDraw.ps.hlsl");

        auto debugDrawRayVert = ResourceManager::Create<ShaderSource>("DebugDrawRay.vs.hlsl",
                                                                      EDITOR_RESOURCE_PATH
                                                                      "/Shaders/DebugDraw/DebugDrawRay.vs.hlsl");
    }
    // Tone Map
    {
        auto toneMap = ResourceManager::Create<ShaderSource>("Aces.cs.hlsl",
                                                             EDITOR_RESOURCE_PATH
                                                             "/Shaders/PostProcess/ToneMapping/Aces.cs.hlsl");
    }

   

    // skybox
    {
        auto skyboxVert = ResourceManager::Create<ShaderSource>("Skybox.vs.hlsl",
                                                                EDITOR_RESOURCE_PATH "/Shaders/Skybox/Skybox.vs.hlsl");

        auto skyboxFrag = ResourceManager::Create<ShaderSource>("Skybox.ps.hlsl",
                                                                EDITOR_RESOURCE_PATH "/Shaders/Skybox/Skybox.ps.hlsl");
    }*/
}

void Editor::HandleAsyncTask()
{
    if(m_HasFinish.load(std::memory_order_acquire))
    {
        m_AfterImportFunc();
        m_ImportThread.reset();
        m_HasFinish = false;
    }
}

void Editor::Init(const PC_CORE::AppCreateInfo& _appCreateInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);

    LoadFromInitFiles();
    CompileShader();

    App::Init(_appCreateInfo);
    IMGUIContext.Init(RenderHarwareInteface, MainWindow.GetHandle());
    InitTestScene();
    InitEditor();

    for (auto& f : m_FuturInits)
    {
        f.wait();
    }
}

void Editor::Destroy()
{
    PERF_REGION_SCOPED;

    // editor window need core
    for (auto& i : editorWindows)
        i.reset();

    IMGUIContext.Destroy();

    App::Destroy();
}

void Editor::UpdateEditor()
{
    PERF_REGION_SCOPED;
    //static bool open = true;
    //ImGui::ShowDemoWindow(&open);

    dockSpace.BeginDockSpace();
    ImGui::PushFont(editorData.editorFont.normal); // push normal font

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("SaveScene"))
            {
                Level& l = World::GetWorld()->level;
                //Serializer::Serialize(l,"TestScene.map");
            }
            if (ImGui::MenuItem("LoadScene"))
            {
                Level& l = World::GetWorld()->level;
                //Serializer::DeSerialize(&l,"TestScene.map");
            }
            ImGui::EndMenu();
        }
        /*
        if (ImGui::BeginMenu("Rendering"))
        {
            auto l = [&](std::shared_ptr<Resource> _shader)
            {
                if (ImGui::MenuItem(_shader->Name.c_str()))
                {
                   RenderHarwareInteface.GetRhiContext().WaitIdle();
                    _shader->Reload();
                    // reload shader
                }
            };

            ResourceManager::ForEach(Reflector::GetTypeKey<ShaderSource>(), l);

            ImGui::EndMenu();
        }
        */
        ImGui::EndMenuBar();
    }


    {
        PERF_REGION_SCOPED_NAMED("Update Windows");
        for (auto& editorWindow : editorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
        }
    }

    for (auto& sub : editorSubSystems)
        sub->Update();

    EditorCommandUpdate();
    ImGui::PopFont();
    dockSpace.EndDockSpace();
}

void Editor::RewindCommand()
{
    if (editorCommands.empty())
        return;

    editorCommands.pop_back();
}

void Editor::InitTestScene()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);
    PC_LOG("InitTestScene...")
        
    m_ImportThread.reset(new std::jthread([&]() {
        Utils::SetThreadName("ImportThread");
        AssetsImporter.ImportModel(RenderHarwareInteface, ThreadPool, editorData.projectPath / "Assets/Meshs/Sponza/glTF/Sponza.gltf");
        m_HasFinish.store(true, std::memory_order_release);
        }));
    
    m_AfterImportFunc = [this]()
        {
            auto& level = World::GetWorld()->level;

            PC_CORE::ObjectPtr<PC_CORE::Rendering::Material> material = ResourceManager::Create<PC_CORE::Rendering::Material>("BaseAlbedo");

            material->SetAlbedoTexture(AssetsImporter.GetTextures().at("5792855332885324923.jpg").second.Lock());

            material->Build();


            EntityId Cube = level.CreateEntity("Cube");
            level.AddComponent<Transform>(Cube);
            level.AddComponent<StaticMeshComponent>(Cube);
            Transform* t = &level.GetComponent<Transform>(Cube);
            t->Position = Tbx::Vector3d(0.0f, 0.0f, 0.0f);
            t->Scale = Tbx::Vector3d(1.0f, 1.0f, 1.0f);
            StaticMeshComponent* s = &level.GetComponent<StaticMeshComponent>(Cube);
            s->material = material;

            const std::vector<PC_CORE::ObjectPtr<PC_CORE::StaticMesh>>& mesh = AssetsImporter.GetStaticMeshes();
            s->staticMesh = mesh[0];

            {
                EntityId pointLight = level.CreateEntity("PointLight");
                level.AddComponent<Transform>(pointLight);
                level.AddComponent<PointLight>(pointLight);
                Transform* t = &level.GetComponent<Transform>(pointLight);
                t->Position = Tbx::Vector3d(0.0f, 2.5f, 0.0f);
                t->Scale = Tbx::Vector3d(1.0f, 1.0f, 1.0f);

                PointLight& p = level.GetComponent<PointLight>(pointLight);
                p.intensity = 5.f;
            }
        };

  
    }
  

void Editor::DestroyTestScene()
{
    if (std::holds_alternative<EntityId>(selectedObject))
        selectedObject = std::monostate();
}

void Editor::OnRender(PC_CORE::CommandList* _Cmd)
{
    for (auto& editorWindow : editorWindows)
        editorWindow->Render(PrimaryCommandBuffer.get());
    for (auto& sub : editorSubSystems)
        sub->Render();

    IMGUIContext.Render(PrimaryCommandBuffer.get());
}

void Editor::Run(bool* _appShouldClose)
{
    // begin game thread
    while (!MainWindow.ShouldClose())
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Editor);

        CoreIo.PoolEvent();
        MainWindow.PoolEvents();
        Time::UpdateTime();


        IMGUIContext.NewFrame();
        HandleAsyncTask();
        WorldTick(Time::DeltaTime());
        UpdateEditor();
        RenderFrame();
        
        PERF_FRAME_MARK;
    }

    RenderHarwareInteface.GetRhiContext().WaitIdle();
}

void Editor::InitEditor()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);


    {
        PC_LOG("InitEditorWindow...")
        editorWindows.push_back(std::make_unique<EditWorldWindow>(*this, "Scene"));
        editorWindows.push_back(std::make_unique<Inspector>(*this, "Inspector"));
        editorWindows.push_back(std::make_unique<Hierachy>(*this, "Hierachy"));
        editorWindows.push_back(std::make_unique<SceneButton>(*this, "SceneButton"));
        editorWindows.push_back(std::make_unique<ResourceBrowserWindow>(*this, "ResourceBrowser"));
    }


    {
        PC_LOG("InitEditorSystem")
        m_EditorRenderer = EditorRenderer(*this);
        m_EditorRenderer.PushCustomCommand();
    }

    {
        ImGuiIO& io = ImGui::GetIO();

        PC_LOG("Load Font")
        auto l = [&](EditorFont* _f, const char* _fontPath)
        {
            _f->tiny = io.Fonts->AddFontFromFileTTF(_fontPath, 11.f);
            _f->small = io.Fonts->AddFontFromFileTTF(_fontPath, 13.f);
            _f->normal = io.Fonts->AddFontFromFileTTF(_fontPath, 15.f);
            _f->big = io.Fonts->AddFontFromFileTTF(_fontPath, 16.f);
            _f->veryBig = io.Fonts->AddFontFromFileTTF(_fontPath, 21.f);
        };

        l(&editorData.editorFont, EDITOR_RESOURCE_PATH"/Font/Verdana.ttf");
        l(&editorData.editorFontItalic, EDITOR_RESOURCE_PATH"/Font/Verdana-Italic.ttf");

        unsigned char* tex_pixels = nullptr;
        int tex_w = 0, tex_h = 0;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
        io.FontDefault = editorData.editorFont.big;
    }

}

void Editor::EditorCommandUpdate()
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Editor);

        RewindCommand();
    }
}
