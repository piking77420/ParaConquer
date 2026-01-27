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
#include "ImguiHelper.h"

#include "ImguiReflectedObject.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;



Editor::Editor()
    : m_EditorThreadPool("Editor Thread Pool", std::max(1u, std::min(2u, std::thread::hardware_concurrency())))
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


    // RenderSettings
    {
        JsonSerializer s;
        if (s.OpenFile("RenderSettings.ini", Serializer::SerializeOperation::DeSerialize))
        {
            s.DeSerialize<Rendering::RenderSettings>(&RenderSettings);
            s.CloseFile();
        }
        
    }

}

void Editor::SaveInitFiles()
{
    if (editorData.projectPath.empty())
        return;

    EditorIniFile editorIniFile;
    editorIniFile.projectPath = editorData.projectPath.generic_string();

    {
        JsonSerializer s;
        s.OpenFile(std::string(EditorIniFileName), Serializer::SerializeOperation::Serialize);
        s.Serialize<EditorIniFile>(editorIniFile);
        s.CloseFile();
    }
    
    {
        JsonSerializer s;
        s.OpenFile("RenderSettings.ini", Serializer::SerializeOperation::Serialize);
        s.Serialize<Rendering::RenderSettings>(RenderSettings);
        s.CloseFile();
    }


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

    // Tone Map
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("Aces.cs.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/PostProcess/ToneMapping/Aces.cs.hlsl");
            }));
    }

    // Mesh Shader Traingle
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("MeshShaderTriangle.ms.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/MeshShader/MeshShaderTriangle.ms.hlsl");
            }));

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("MeshShaderTriangle.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/MeshShader/MeshShaderTriangle.ps.hlsl");
            }));
    }

    // Mesh Shader
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("MeshShaderMeshlet.ms.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/MeshShader/MeshShaderMeshlet.ms.hlsl");
            }));

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("MeshShaderMeshlet.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/MeshShader/MeshShaderMeshlet.ps.hlsl");
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


   

    // skybox
    {
        auto skyboxVert = ResourceManager::Create<ShaderSource>("Skybox.vs.hlsl",
                                                                EDITOR_RESOURCE_PATH "/Shaders/Skybox/Skybox.vs.hlsl");

        auto skyboxFrag = ResourceManager::Create<ShaderSource>("Skybox.ps.hlsl",
                                                                EDITOR_RESOURCE_PATH "/Shaders/Skybox/Skybox.ps.hlsl");
    }*/
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

        if (ImGui::BeginMenu("RenderSettings"))
        {
            ImGui::Text("MSAA : ");
            ImGui::SameLine();
            {
                ImGui::ScopedFont(editorData.editorFont.tiny);
                ImGuiReflection::SelectEnum<decltype(PC_CORE::Rendering::RenderSettings::MSAASampleCount)>(&RenderSettings.MSAASampleCount);
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

    /*if (testMaterial)
    {


        ImGui::Begin("TestUpdateMaterial");

        auto& m = testMaterial->BeginUpdateMaterialData();
        if (ImGui::ColorPicker4("Color", &m.AlbedoFactors.data[0]))
        {
            testMaterial->UpdateMaterialData();
        }

        ImGui::End();
    }*/


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

void Editor::TempImportModel(const std::filesystem::path& _path)
{
    // TODO STORE ASYNC TASK FUSUTURE AND WIAT IN DESTRUCTOR


    Guid importesGuid{};
    AssetsImporter* ptr = nullptr;
    {
        std::scoped_lock _(AssetImportData._lock);
        ptr = AssetImportData.Imports.emplace_back(new AssetsImporter()).get();
    }

    if (!ptr)
        return;
    importesGuid = ptr->GetGuid();

    {
        TaskHandle ImportMesh = TaskScheduler.NewTask(m_EditorThreadPool,
            [&, Importer = ptr, path = _path]
            ()
            {
                Importer->ImportModel(RenderHarwareInteface, ThreadPool, path);
            });

        if (importesGuid != Guid::Empty())
        {
            TaskHandle CreateStaticMesh = TaskScheduler.NewTask(TaskThread::MainThread,
                [this, impGuid = importesGuid]()
                {
                    ObjectPtr<PC_CORE::StaticMesh> StaticMesh;

                    PERF_REGION_SCOPED;
                    PERF_REGION_COLOR(PerfRegion::EditorResource);
                    {
                        std::scoped_lock _(AssetImportData._lock);
                        auto it = std::ranges::find_if(AssetImportData.Imports, [&](const std::unique_ptr<AssetsImporter>& _Importer) {
                            return _Importer->GetGuid() == impGuid;
                            });
                        if (it != AssetImportData.Imports.end() && (*it)->GetSuccess())
                        {
                            StaticMesh = (*it)->GetStaticMeshes();
                        }
                    }

                    auto& level = World::GetWorld()->level;
                    if (StaticMesh)
                    {
                        EntityId staticMesh = level.CreateEntity(StaticMesh->Name);
                        level.AddComponent<Transform>(staticMesh);
                        level.AddComponent<StaticMeshComponent>(staticMesh);
                        Transform* t = &level.GetComponent<Transform>(staticMesh);
                        t->Position = Tbx::Vector3d(0.0f, 0.0f, 0.0f);
                        t->Scale = Tbx::Vector3d(1.0f, 1.0f, 1.0f);
                        StaticMeshComponent* s = &level.GetComponent<StaticMeshComponent>(staticMesh);
                        s->staticMesh = StaticMesh;
                    }
                }, { ImportMesh });
        }

        TaskScheduler.Lauch(ImportMesh);
    }
}


void Editor::InitTestScene()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);
    PC_LOG("InitTestScene...")


    {
        auto& level = World::GetWorld()->level;
        EntityId dirLight = level.CreateEntity("DirLight");
        level.AddComponent<Transform>(dirLight);
        level.AddComponent<DirLight>(dirLight);
        Transform* t = &level.GetComponent<Transform>(dirLight);
        t->Position = Tbx::Vector3d(0.0f, 2.5f, 0.0f);
        t->Scale = Tbx::Vector3d(1.0f, 1.0f, 1.0f);

        DirLight& p = level.GetComponent<DirLight>(dirLight);
        p.intensity = 1.f;
        p.color = Tbx::Vector3f(1.f, 1.f, 1.f);
    }
    
    //
    TempImportModel((editorData.projectPath / "Assets/Meshs/Sponza/glTF/Sponza.gltf"));
    TempImportModel((editorData.projectPath / "Assets/Meshs/Entity_LionDog_high.fbx"));
    TempImportModel((editorData.projectPath / "Assets/Meshs/DamagedHelmet/glTF/DamagedHelmet.gltf"));

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
        DequeuMainThreadTask();
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
