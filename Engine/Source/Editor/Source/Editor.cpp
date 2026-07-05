#include "Editor.hpp"

#include <numbers>
#include <imgui_internal.h>
#include <PerfRegion.hpp>
#include <ProjectSettingsWindow.hpp>
#include <Resources/ResourceManager.hpp>

#include "Resources/ResourceManager.hpp"
#include "EditWorldWindow.hpp"
#include "Hierachy.hpp"
#include "Inspector.hpp"
#include "ResourceBrowserWindow.hpp"
#include "SceneButton.hpp"
#include "Time/CoreTime.hpp"
#include "WorldViewWindow.hpp"
#include "EditorFiles.hpp"
#include "ImguiHelper.h"
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
#include "Thread/ThreadUtils.hpp"
#include "World/StaticMeshComponent.hpp"

#include "ImguiReflectedObject.hpp"
#include <Io/DDSImageLoader.hpp>

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;



Editor::Editor(const PC_CORE::AppCreateInfo& _AppCreateInfo) 
    : PC_CORE::App(_AppCreateInfo)
    , m_EditorThreadPool("Editor Thread Pool", std::max(1u, std::min(2u, std::thread::hardware_concurrency())))
{

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);

    if (instance != nullptr)
    {
        PC_LOGERROR("Editor instance is not nullptr");
        exit(-1);
    }
    instance = this;

    LoadFromInitFiles();
    CompileShader();

    IMGUIContext.Init(RenderHarwareInteface, MainWindow.GetHandle());
    Renderer.OnSwapchainPass = [&](PC_CORE::CommandList& List) {
        IMGUIContext.Render(&List);
    };
    Renderer.OnRender = [&](PC_CORE::CommandList& List) {
        OnRender(&List);
        };
    InitTestScene();
    InitEditor();

    for (auto& f : m_FuturInits)
    {
        f.wait();
    }

    // test dds
    //DDSImageLoader dds("C:/Project/ParaConquerGame/Assets/Meshs/Bistro_v5_2/Textures/MASTER_Roofing_Shingle_Grey_BaseColor.dds");

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

void Editor::EditorOnlyShader()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);

    PC_LOG("EditorOnlyShader...")
    m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
    ResourceManager::Create<ShaderSource>("DebugDrawInstanced.vs.hlsl",
        EDITOR_RESOURCE_PATH
        "/Shaders/DebugDraw/DebugDraw.vs.hlsl",
        ShaderFeatureFlagBits::Instanced);
        }));

    m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
        ResourceManager::Create<ShaderSource>("DebugDrawFrustum.vs.hlsl",
            EDITOR_RESOURCE_PATH
            "/Shaders/DebugDraw/DebugDraw.vs.hlsl",
            ShaderFeatureFlagBits::Frustum);
        }));

    m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
        ResourceManager::Create<ShaderSource>("DebugDraw.ps.hlsl",
            EDITOR_RESOURCE_PATH
            "/Shaders/Lit.ps.hlsl"
            , ShaderFeatureFlagBits::UseColor);
        }));


    // Draw Meshlet Bound
    m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
        ResourceManager::Create<ShaderSource>("DrawMeshletBound.as.hlsl",
            EDITOR_RESOURCE_PATH
            "/Shaders/Meshlet/DrawMeshletBound.as.hlsl"
            , ShaderFeatureFlagBits::UseColor);
        }));

    m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
        ResourceManager::Create<ShaderSource>("DrawMeshletBound.ms.hlsl",
            EDITOR_RESOURCE_PATH
            "/Shaders/Meshlet/DrawMeshletBound.ms.hlsl"
            , ShaderFeatureFlagBits::UseColor);
        }));
}

void Editor::CompileShaderDebugView()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);

    PC_LOG("CompileShaderDebugView...")

    // Draw Triangle
    { 
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("DrawMeshTriangle.vs.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/TriangleBased.vs.hlsl",
                ShaderFeatureFlagBits::UseColor | ShaderFeatureFlagBits::DrawTriangle);
            }));

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("DrawMeshTriangle.ps.hlsl",
                EDITOR_RESOURCE_PATH"/Shaders/Lit.ps.hlsl",
                ShaderFeatureFlagBits::UseColor | ShaderFeatureFlagBits::DrawTriangle);
            }));
    }

    // Draw Triangle Meshlet
    {

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("DrawTriangleMeshlet.ms.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Meshlet/DrawMeshlet.ms.hlsl",
                ShaderFeatureFlagBits::UseColor | ShaderFeatureFlagBits::DrawTriangle);
            }));

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("DrawTriangleMeshlet.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Lit.ps.hlsl",
                ShaderFeatureFlagBits::UseColor | ShaderFeatureFlagBits::DrawTriangle);
            }));
    }

    
    // Draw Meshlet Color
    {

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("DrawMeshletColor.ms.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Meshlet/DrawMeshlet.ms.hlsl",
                ShaderFeatureFlagBits::UseColor);
            }));

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("DrawMeshletColor.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Lit.ps.hlsl",
                ShaderFeatureFlagBits::UseColor);
            }));
    }
}


void Editor::CompileShader()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);


    PC_LOG("CompileShader...")
    EditorOnlyShader();

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
            ResourceManager::Create<ShaderSource>("DrawMeshlet.as.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Meshlet/DrawMeshlet.as.hlsl");
            }));
    }

    // Lit Triangle
	{
        // only compile primite shader
		m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
			ResourceManager::Create<ShaderSource>("Forward.vs.hlsl",
				EDITOR_RESOURCE_PATH
				"/Shaders/TriangleBased.vs.hlsl",
                ShaderFeatureFlagBits::Lit | ShaderFeatureFlagBits::UseUV | ShaderFeatureFlagBits::UseNormalMap);
			}));
	}

    // Lit Triangle meshelet
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("ForwardMeshlet.ms.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Meshlet/DrawMeshlet.ms.hlsl",
                ShaderFeatureFlagBits::Lit | ShaderFeatureFlagBits::UseUV | ShaderFeatureFlagBits::UseNormalMap);
            }));   

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("ForwardLit.ps.hlsl",
                EDITOR_RESOURCE_PATH"/Shaders/Lit.ps.hlsl",
                ShaderFeatureFlagBits::Lit | ShaderFeatureFlagBits::UseUV | ShaderFeatureFlagBits::UseNormalMap);
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

    // Cube Map
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("CubeMap.vs.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/CubeMap.vs.hlsl",
                ShaderFeatureFlagBits::UseUV);
            }));
    }

    // Equirectacular to skybox
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("EquirectangularToCubeMap.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Ibl/EquirectangularToCubeMap.ps.hlsl");
            }));
    }

    // Irradiance Convolution
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("IrradianceConvolution.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Ibl/IrradianceConvolution.ps.hlsl");
            }));
    }

    // Skybox
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("Skybox.vs.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/CubeMap.vs.hlsl",
                ShaderFeatureFlagBits::UseUV | ShaderFeatureFlagBits::SkyboxForceFarDepth);
            }));

        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("Skybox.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Skybox.ps.hlsl",
                ShaderFeatureFlagBits::UseUV);
            }));
    }

    // PrefilterMap
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("PrefilterEnvironement.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Ibl/Prefiltering.ps.hlsl",
                ShaderFeatureFlagBits::UseUV);
            }));
    }

    // BRDFLUT
    {
        m_FuturInits.emplace_back(ThreadPool.Enqueue([]()->void {
            ResourceManager::Create<ShaderSource>("BRDFLUT.ps.hlsl",
                EDITOR_RESOURCE_PATH
                "/Shaders/Ibl/BRDFLUT.ps.hlsl");
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

    CompileShaderDebugView();
}

void Editor::Destroy()
{
    PERF_REGION_SCOPED;

    {
        std::scoped_lock _(AssetImportData._lock);
        AssetImportData.Imports.clear();
    }
    // editor window need core
    for (auto& EditorWindow : EditorWindows)
        EditorWindow.reset();

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

            if (ImGui::MenuItem("ProjectSetting"))
            {
                m_ProjectSettingsWindow->isOpen = !m_ProjectSettingsWindow->isOpen;
                if (m_ProjectSettingsWindow->isOpen)
                    m_ProjectSettingsWindow->PushUpFrontAndFocus();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("RenderSettings"))
        {
            ImGuiReflection::DrawEnumMenue("MSAA", &RenderSettings.MSAASampleCount);
            if (ImGuiReflection::DrawEnumMenue("DebugView", &editorData.DebugView))
            {
                if (editorData.DebugView == DebugView::Meshlet &&
                    editorData.ProjectSettings.RenderMode == PC_CORE::Rendering::RenderMode::TriangleBased)
                {
                    editorData.DebugView = DebugView::Triangle;
                }

                m_EditorWorldWindow->OnRenderModeDirty();
            }

            ImGui::Checkbox("FreezeCameraFrustum", &editorData.FreezeFrustum);
            ImGui::Checkbox("DrawCameraFrustum", &editorData.DrawFrustum);

            if (editorData.ProjectSettings.RenderMode == PC_CORE::Rendering::RenderMode::ClusterBased)
            {
                if (ImGui::Checkbox("DrawMeshletBounds", &editorData.DrawMesheltBounds))
                {
                    m_EditorWorldWindow->OnRenderModeDirty();
                }

                if (ImGui::Checkbox("CullMeshlet", &editorData.CullMeshlet))
                {

                }
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
        for (auto& EditorWindow : EditorWindows)
        {
            if (!EditorWindow->isOpen)
                continue;
            
            EditorWindow->Begin();
            EditorWindow->Update();
            EditorWindow->End();
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

AssetsImporter* Editor::NewAssetsImporter()
{
    AssetsImporter* ptr = nullptr;
    {
        std::scoped_lock _(AssetImportData._lock);
        ptr = AssetImportData.Imports.emplace_back(new AssetsImporter()).get();
    }

    return ptr;
}

void Editor::TempImport(const std::filesystem::path& _path)
{
    // TODO STORE ASYNC TASK FUSUTURE AND WIAT IN DESTRUCTOR    
    AssetsImporter* AssetsImporter = NewAssetsImporter();

    if (!AssetsImporter)
        return;
    const std::string ext = _path.extension().generic_string();
    if (ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".obj" || ext == ".FBX")
    {
        AssetsImporter->ImportModel(RenderHarwareInteface, ThreadPool, _path);
    }
    else if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".hdr")
    {
        AssetsImporter->ImportTexture(RenderHarwareInteface, _path);
    }
    else
    {
        PC_LOGERROR("Invalid path extension for this path {}", _path.generic_string());
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

    static constexpr size_t SphereCountPerAxis = 10;
    static constexpr double SpaceBetweenSphere = 50.0;

    {
        //TempImport((editorData.projectPath / "Assets/Textures/papermill.hdr"));
        //World.Environement.FromEnvironementMap(*this, ResourceManager::Get<PC_CORE::Texture2D>("papermill.hdr"));
        
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {
                //TempImport((editorData.projectPath / "Assets/Meshs/Bistro/Bistro_v5_2/san_giuseppe_bridge_4k.hdr")); 
                TempImport((editorData.projectPath / "Assets/Textures/pisa.hdr"));
            });
        auto TaskHandle2 = TaskScheduler.NewTask(Thread::TaskNode::Thread::MainThread,
            [&]() {
                World.Environement.FromEnvironementMap(*this, ResourceManager::Get<PC_CORE::Texture2D>("pisa.hdr")); },
            { TaskHandle });
        TaskScheduler.Lauch(TaskHandle); // then ask to create a cube map "3D texture" and ask to render to create an cube map from it with barrier etc*/
    }
#if 0
    {
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/obj/sphere.obj")); });

        auto TaskHandle2 = TaskScheduler.NewTask(Thread::TaskNode::Thread::MainThread, [&]()
            {
                auto& level = World::GetWorld()->level;

                constexpr size_t RowCount = 7;     // Metallic steps
                constexpr size_t ColumnCount = 7;  // Roughness steps
                constexpr float Spacing = 2.5f;

                for (size_t row = 0; row < RowCount; row++)
                {
                    const float Metallic = (float)row / (float)RowCount;

                    for (size_t col = 0; col < ColumnCount; col++)
                    {
                        const float Roughness = std::clamp(
                            static_cast<float>(col) / static_cast<float>(ColumnCount),
                            0.05f,
                            1.0f
                        );

                        std::string MaterialFormat = std::format(
                            " Roughness {:.2f} Metallic {:.2f}",
                            Roughness,
                            Metallic
                        );

                        const EntityId id = level.CreateEntity(
                            std::string("Sphere") + MaterialFormat
                        );

                        level.AddComponent<Transform>(id);
                        Transform& t = level.GetComponent<Transform>(id);

                        t.Position = Tbx::Vector3d(
                            ((float)col - ((float)ColumnCount / 2.0f)) * Spacing,
                            ((float)row - ((float)RowCount / 2.0f)) * Spacing,
                            -2.0f
                        );

                        level.AddComponent<StaticMeshComponent>(id);
                        StaticMeshComponent& smc = level.GetComponent<StaticMeshComponent>(id);

                        smc.staticMesh = ResourceManager::Get<StaticMesh>("sphere.obj");

                        ObjectPtr<PC_CORE::Rendering::Material> Material =
                            ResourceManager::Create<PC_CORE::Rendering::Material>(
                                std::string("PBR Material") + MaterialFormat
                            );

                        Material->SetRoughnessFactor(Roughness);
                        Material->SetMetallicFactor(Metallic);

                        Material->SetAlbedoFactor(Tbx::Vector4f(1.f, 0.f, 0.f, 1.0f));

                        Material->Build();

                        smc.materials.emplace_back() = Material;
                    }
                }
                }, { TaskHandle });
                TaskScheduler.Lauch(TaskHandle);    
            }
#endif

#if 0
    {
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/obj/sphere.obj")); });

        auto TaskHandle2 = TaskScheduler.NewTask(Thread::TaskNode::Thread::MainThread, [&]()
            {
                auto& level = World::GetWorld()->level;

                constexpr size_t ObjectCount = 10;

                for (size_t row = 0; row < ObjectCount; row++)
                {
                    const float Metallic = std::clamp((float)row / (float)ObjectCount, 0.005f, 1.0f);
                    const float Roughness = 1.0f - std::clamp((float)row / (float)ObjectCount, 0.005f, 1.0f);

                    std::string MaterialFormat = std::format(
                        " Roughness {:.2f} Metallic {:.2f}",
                        Roughness,
                        Metallic
                    );

                    const EntityId id = level.CreateEntity(
                        std::string("Sphere") + MaterialFormat
                    );

                    level.AddComponent<Transform>(id);
                    Transform& t = level.GetComponent<Transform>(id);

                    t.Position = Tbx::Vector3d(
                        -2.0,
                        0.0,
                        (float(row - (ObjectCount / 2.0f)) * 2.15f)
                    );

                    level.AddComponent<StaticMeshComponent>(id);
                    StaticMeshComponent& smc = level.GetComponent<StaticMeshComponent>(id);

                    smc.staticMesh = ResourceManager::Get<StaticMesh>("sphere.obj");

                    ObjectPtr<PC_CORE::Rendering::Material> Material =
                        ResourceManager::Create<PC_CORE::Rendering::Material>(
                            std::string("PBR Material") + MaterialFormat
                        );

                    Material->SetRoughnessFactor(Roughness);
                    Material->SetMetallicFactor(Metallic);

                    Material->SetAlbedoFactor(Tbx::Vector4f(1.0f, 0.765557f, 0.336057f, 1.0f));

                    Material->Build();

                    smc.materials.emplace_back() = Material;
                    
                }
            }, { TaskHandle });
        TaskScheduler.Lauch(TaskHandle);
    }
#endif

#if 0
    {
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/DamagedHelmet/glTF/DamagedHelmet.gltf")); });

        auto CreateStaticMesh = TaskScheduler.NewTask(PC_CORE::Thread::TaskNode::Thread::MainThread,
            [&]()
            {
                auto& level = World::GetWorld()->level;

                const EntityId id = level.CreateEntity(std::string("Bistro"));
                level.AddComponent<Transform>(id);
                Transform& t = level.GetComponent<Transform>(id);
                t.Position = Tbx::Vector3d(0.0, 0.0, 0.0);

                level.AddComponent<StaticMeshComponent>(id);
                StaticMeshComponent& smc = level.GetComponent<StaticMeshComponent>(id);
                smc.staticMesh = ResourceManager::Get<StaticMesh>("DamagedHelmet.gltf");
                if (auto l = smc.staticMesh.Lock())
                    smc.materials = l->GetBaseMaterial();

            }, { TaskHandle });

        TaskScheduler.Lauch(TaskHandle);
    }

    {
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/obj/sphere.obj")); });
        auto TaskHandle2 = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/obj/cube.obj")); });
        TaskScheduler.Lauch(TaskHandle);
        TaskScheduler.Lauch(TaskHandle2);
    }
#endif

#if 1
    {
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/Sponza/glTF/Sponza.gltf")); });

        auto CreateStaticMesh = TaskScheduler.NewTask(PC_CORE::Thread::TaskNode::Thread::MainThread,
            [&]()
            {
                auto& level = World::GetWorld()->level;

                const EntityId id = level.CreateEntity(std::string("Bistro"));
                level.AddComponent<Transform>(id);
                Transform& t = level.GetComponent<Transform>(id);
                t.Position = Tbx::Vector3d(0.0, 0.0, 0.0);

                level.AddComponent<StaticMeshComponent>(id);
                StaticMeshComponent& smc = level.GetComponent<StaticMeshComponent>(id);
                smc.staticMesh = ResourceManager::Get<StaticMesh>("Sponza.gltf");
                if (auto l = smc.staticMesh.Lock())
                    smc.materials = l->GetBaseMaterial();

            }, { TaskHandle });

        TaskScheduler.Lauch(TaskHandle);
    }

    {
        auto TaskHandle = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/obj/sphere.obj")); });
        auto TaskHandle2 = TaskScheduler.NewTask(m_EditorThreadPool,
            [&]() {TempImport((editorData.projectPath / "Assets/Meshs/obj/cube.obj")); });
        TaskScheduler.Lauch(TaskHandle);
        TaskScheduler.Lauch(TaskHandle2);
    }
#endif

    

    //TempImportModel((editorData.projectPath / "Assets/Meshs/Sponza/glTF/Sponza.gltf"));
   

        
    //TempImportModel((editorData.projectPath / "Assets/Meshs/Entity_LionDog_high.fbx"), true);
    //TempImportModel((editorData.projectPath / "Assets/Meshs/DamagedHelmet/glTF/DamagedHelmet.gltf"), true);
    //TempImportModel((editorData.projectPath / "Assets/Meshs/Horse/horse_statue_01_4k.glb"), true);
    //TempImportModel((editorData.projectPath / "Assets/Meshs/obj/dragon.fbx"), true);
    //TempImportModel((editorData.projectPath / "Assets/Meshs/obj/chinesedragon.gltf"), true);

    //TempImportModel((editorData.projectPath / "Assets/Meshs/StandfordBunny.obj"), true);
    //TempImportModel((editorData.projectPath / "Assets/SKM_Manny_Simple.FBX"), true);
}
  

void Editor::DestroyTestScene()
{
    if (std::holds_alternative<EntityId>(selectedObject))
        selectedObject = std::monostate();
}

void Editor::OnRender(PC_CORE::CommandList* _Cmd)
{
    for (auto& EditorWindow : EditorWindows)
        EditorWindow->Render();
    for (auto& sub : editorSubSystems)
        sub->Render();

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
        IMGUIContext.NewFrame();
        Time::UpdateTime();
        DequeuMainThreadTask();
        WorldTick(Time::DeltaTime());
        UpdateEditor();
        Renderer.RenderFrame();
        PERF_FRAME_MARK;
    }
    RenderHarwareInteface.GetRhiContext().WaitIdle();
}

void Editor::InitEditor()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Editor);


    {
        PC_LOG("InitEditorWindows...")
        m_ProjectSettingsWindow = EditorWindows.emplace_back(std::make_unique<ProjectSettingsWindow>(*this, "ProjectSettings")).get(); // order is important
        auto WorldWindowPtr = std::make_unique<EditWorldWindow>(*this, "Scene");
        m_EditorWorldWindow = WorldWindowPtr.get();
        EditorWindows.emplace_back(std::move(WorldWindowPtr));
        EditorWindows.push_back(std::make_unique<Inspector>(*this, "Inspector"));
        EditorWindows.push_back(std::make_unique<Hierachy>(*this, "Hierachy"));
        EditorWindows.push_back(std::make_unique<SceneButton>(*this, "SceneButton"));
        EditorWindows.push_back(std::make_unique<ResourceBrowserWindow>(*this, "ResourceBrowser"));
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
