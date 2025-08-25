#include <thread> 
#include <Chrono>
#include <Iostream>


#include <PerfRegion.hpp>
#include <Imgui/imgui_internal.h>


#include "Editor.hpp"
#include "Resources/ResourceManager.hpp"


#include "AssetBrowserWindow.hpp"
#include "EditWorldWindow.hpp"
#include "Hierachy.hpp"
#include "Inspector.hpp"
#include "SceneButton.hpp"
#include "WorldViewWindow.hpp"
#include "Time/CoreTime.hpp"
#include <Resources/ResourceManager.hpp>
#include "Rendering/Light.hpp"
#include "Io/CoreIo.hpp"
#include "Io/ImguiContext.h"
#include "Physics/RigidBody.hpp"
#include "Rendering/Material.hpp"
#include "Resources/ShaderSource.hpp"
#include "World/StaticMesh.hpp"
#include "Serialize/Serializer.h"

#include <Windows.h>      // For common windows data types and function headers
#define STRICT_TYPED_ITEMIDS
#include <Objbase.h>      // For COM headers
#include <Shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <Shlwapi.h>
#include <Knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <Propvarutil.h>  // for PROPVAR-related functions
#include <Propkey.h>      // for the Property key APIs/datatypes
#include <Propidl.h>      // for the Property System APIs
#include <Strsafe.h>      // for StringCchPrintfW
#include <Shtypes.h>      // for COMDLG_FILTERSPEC
#include <New>
#include <Shobjidl.h>  // For IFileDialogEvents

#include "Serialize/Iseriazable.h"
#include <Random> // pour std::mt19937 et std::uniform_real_distribution

#include "Rendering/RenderSystem.hpp"

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

	editorData.graphicApi = PC_CORE::GraphicAPI::Vulkan;

}

Editor::~Editor()
{
	instance = nullptr;
}

void Editor::CompileShader()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	PC_LOG("CompileShader...")

	auto forwardVert = ResourceManager::Create<ShaderSource>("Forward.vs.hlsl", EDITOR_RESOURCE_PATH "/Shaders/Forward/Forward.vs.hlsl");

	auto forwardFrag = ResourceManager::Create<ShaderSource>("Forward.ps.hlsl",EDITOR_RESOURCE_PATH "/Shaders/Forward/Forward.ps.hlsl");
	// sprite
	{
		auto spriteVert = ResourceManager::Create<ShaderSource>("DrawSprite.vs.hlsl", EDITOR_RESOURCE_PATH "/Shaders/DrawSprite/DrawSprite.vs.hlsl");
		auto spriteFrag = ResourceManager::Create<ShaderSource>("DrawSprite.ps.hlsl", EDITOR_RESOURCE_PATH "/Shaders/DrawSprite/DrawSprite.ps.hlsl");
	}

	// geometry buffer
	{
		auto geometryVert = ResourceManager::Create<ShaderSource>("Geometry.vs.hlsl",
			EDITOR_RESOURCE_PATH "/shaders/Geometry/Geometry.vs.hlsl");

		auto geometryFrag = ResourceManager::Create<ShaderSource>("Geometry.ps.hlsl",
			EDITOR_RESOURCE_PATH "/shaders/Geometry/Geometry.ps.hlsl");
	}

	// deferred
	{
		auto deferredFrag = ResourceManager::Create<ShaderSource>("Deferred.ps.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/Deferred/Deferred.ps.hlsl");
	}

	{ // DebugDraw
		auto debugDrawVert = ResourceManager::Create<ShaderSource>("DebugDraw.vs.hlsl", EDITOR_RESOURCE_PATH "/Shaders/DebugDraw/DebugDraw.vs.hlsl");

		auto debugDrawFrag = ResourceManager::Create<ShaderSource>("DebugDraw.ps.hlsl", EDITOR_RESOURCE_PATH "/Shaders/DebugDraw/DebugDraw.ps.hlsl");

		auto debugDrawRayVert = ResourceManager::Create<ShaderSource>("DebugDrawRay.vs.hlsl", EDITOR_RESOURCE_PATH "/Shaders/DebugDraw/DebugDrawRay.vs.hlsl");
	}
	// Tone Map
	{
		auto toneMap = ResourceManager::Create<ShaderSource>("Aces.comp.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/PostProcess/ToneMapping/Aces.cs.hlsl");
	}

	{
		auto drawQuadvertex = ResourceManager::Create<ShaderSource>("DrawQuad.vs.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/DrawQuad.vs.hlsl");

		auto sampleSingleTexture = ResourceManager::Create<ShaderSource>("SampleSingleTexture.ps.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/SampleSingleTexture.ps.hlsl");
	}

	// skybox
	{
		auto skyboxVert = ResourceManager::Create<ShaderSource>("Skybox.vs.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/Skybox/Skybox.vs.hlsl");

		auto skyboxFrag = ResourceManager::Create<ShaderSource>("Skybox.ps.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/Skybox/Skybox.ps.hlsl");
	}
}

void Editor::LookForEditorInit()
{
	/*
	namespace fs = std::filesystem;

	// Look for editor Init or create one

	const std::filesystem::path workingDir = std::filesystem::current_path();
	const std::string editorDataInitFile = workingDir.generic_string() + "/" + ParaConquerEditorInitFile;
	const std::filesystem::path editorDataInitFilePath(editorDataInitFile);

	if (!fs::exists(editorDataInitFilePath))
	{
		std::ofstream createFile(editorDataInitFile);

		if (!createFile.is_open())
		{
			// error should be able to create file
			exit(1);
		}
		createFile.close();
	}

	try
	{
		json j = json::parse(editorDataInitFile);

		const std::string projectPath = j[EditorInitDataKeys[(uint8_t)EditorInitData::PROJECT_ABSOLUTE_PATH]];

		if (projectPath.empty() || !fs::exists(std::filesystem::path(projectPath)))
		{
			BasicOpenFile();
		}
	}
	catch (...)
	{
		// select A project folder
		// to do import basic files
		BasicOpenFile();
	}*/

}

void Editor::BasicOpenFile()
{
	std::wstring fileToOpen;
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen = NULL;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath = NULL;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						// Display the file path in a message box
						MessageBoxW(NULL, pszFilePath, L"Selected File", MB_OK);
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}


void Editor::Init()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	const AppCreateInfo appCreateInfo =
	{
		.appName = "Para Conquer Editor",
		.appLogoPath = EDITOR_RESOURCE_PATH "/logo/ParaConquerLogoBlack.png",
		.enableGpuDebug = true,
		.graphicAPI = editorData.graphicApi
	};

	CompileShader();
	m_AssetBrowser.SetPath("Assets");

	gameApp.Init(appCreateInfo);
	IMGUIContext.Init(gameApp.window.GetHandle(), Rhi::GetInstance().GetGraphicsAPI());

	gameApp.renderer.swapChainPassCommandList->RecordFetchCommand([&](CommandList* cmd) {
		cmd->BeginDebugLabel("Imgui Draw", IMGUI_RENDER_DEBUG_COLOR);
		IMGUIContext.Render(cmd);
		cmd->EndDebugLabel();
		});


	// TO AVOID USING A SYSTEM TO GET ENTIES SYGNATURE 
	// TO DO FIND A WAY TO ITERATE OVER A BIT SET OF 100000000 QUICKLY
	//https://en.wikipedia.org/wiki/Van_Emde_Boas_tree
	World::GetWorld()->level.RegisterSystem<PC_CORE::RendererSystem>(&gameApp.renderingWorldData);

	InitTestScene();
	InitEditor();
}

void Editor::Destroy()
{
	PERF_REGION_SCOPED;

	// editor window need core
	for (auto& i : editorWindows)
		i.reset();

	IMGUIContext.Destroy();

	gameApp.Destroy();
}
void Editor::UpdateEditor()
{
	PERF_REGION_SCOPED;
	//static bool open = true;
	//ImGui::ShowDemoWindow(&open);

	dockSpace.BeginDockSpace();
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

		if (ImGui::BeginMenu("Rendering"))
		{
			auto l = [&](std::shared_ptr<Resource> _shader)
				{
					if (ImGui::MenuItem(_shader->name.c_str()))
					{
						Rhi::GetRhiContext()->WaitIdle();
						_shader->Reload();
						// reload shader
					}
				};

			PC_CORE::ResourceManager::ForEach(PC_CORE::Reflector::GetTypeKey<ShaderSource>(), l);

			ImGui::EndMenu();
		}
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
	dockSpace.EndDockSpace();

	{
		PERF_REGION_SCOPED_NAMED("Editor Render");
		m_EditorRenderer.DrawSelectedEntity();

		for (auto& editorWindow : editorWindows)
			editorWindow->Render();
		for (auto& sub : editorSubSystems)
			sub->Render();
	}

}




void Editor::RewindCommand()
{
	if (editorCommands.empty())
		return;

	editorCommands.pop_back();
}

void Editor::InitTestScene()
{
	/*
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);
	PC_LOG("InitTestScene...")


		std::shared_ptr<Material> m1 = ResourceManager::Create<Material>("DiamondBlockMaterial.mat");
	std::shared_ptr<Material> m2 = ResourceManager::Create<Material>("EmerauldBlockMaterial.mat");

	m1->albedo = ResourceManager::Get<Texture2D>("DiamondBlock.jpg");
	m1->Build();


	m2->albedo = ResourceManager::Get<Texture2D>("EmerauldBlock.png");
	m2->Build();

	auto& level = World::GetWorld()->level;



	EntityId sphere = level.CreateEntity("Sphere");
	level.AddComponent<Transform>(sphere);
	level.AddComponent<StaticMesh>(sphere);
	Transform* t = &level.GetComponent<Transform>(sphere);
	t->position = Tbx::Vector3d(0.0f, 0.0f, 0.0f);
	t->scale = Tbx::Vector3d(2.0f, 2.0f, 2.0f);


	StaticMesh* mesh2 = &level.GetComponent<StaticMesh>(sphere);
	mesh2->mesh = ResourceManager::Get<Mesh>("Sphere.obj");
	mesh2->material = m2;


	EntityId pointLight = level.CreateEntity("PointLight");
	level.AddComponent<Transform>(pointLight);
	level.AddComponent<PointLight>(pointLight);
	t = &level.GetComponent<Transform>(pointLight);
	t->position = Tbx::Vector3d(0.0f, 2.5f, 0.0f);
	t->scale = Tbx::Vector3d(1.0f, 1.0f, 1.0f);

	PointLight& p = level.GetComponent<PointLight>(pointLight);
	p.intensity = 5.f;*/
}

void Editor::DestroyTestScene()
{


	if (std::holds_alternative<EntityId>(selectedObject))
		selectedObject = std::monostate();


	//ResourceManager::Delete<Material>("material1");
	//ResourceManager::Delete<Material>("material2");
}

void Editor::Run(bool* _appShouldClose)
{
	// begin game thread
	while (!gameApp.window.ShouldClose())
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Editor);


		gameApp.coreIo.PoolEvent();
		gameApp.window.PoolEvents();
		PC_CORE::Time::UpdateTime();


		IMGUIContext.NewFrame();
		gameApp.WorldTick(PC_CORE::Time::DeltaTime());
		gameApp.renderer.GetRenderingData(gameApp.renderingWorldData);

		// end game thread
		// begin render thread
		gameApp.renderer.BeginFrame(&gameApp.window);
		UpdateEditor();
		gameApp.renderer.SwapBuffers(&gameApp.window);
		PERF_FRAME_MARK;


	}

	Rhi::GetRhiContext()->WaitIdle();
}

void Editor::InitEditor()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);
	PC_LOG("InitEditorWindow...")

		editorWindows.push_back(std::make_unique<EditWorldWindow>(*this, "Scene"));
	editorWindows.push_back(std::make_unique<Inspector>(*this, "Inspector"));
	editorWindows.push_back(std::make_unique<Hierachy>(*this, "Hierachy"));
	editorWindows.push_back(std::make_unique<SceneButton>(*this, "SceneButton"));
	editorWindows.push_back(std::make_unique<AssetBrowserWindow>(*this, "AssetBrowser"));

	PC_LOG("InitEditorSystem")
		m_EditorRenderer = EditorRenderer(*this);
	m_EditorRenderer.PushCustomCommand();
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
