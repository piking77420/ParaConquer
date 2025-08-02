#include <thread> 
#include <chrono>
#include <iostream>


#include <perf_region.hpp>
#include <glslang/Include/glslang_c_interface.h>
#include <Imgui/imgui_internal.h>
//#include <Json/json.hpp>


#include "editor.hpp"
#include "resources/resource_manager.hpp"

#include "asset_browser.hpp"
#include "edit_world_window.hpp"
#include "hierachy.hpp"
#include "inspector.hpp"
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

#include <windows.h>      // For common windows data types and function headers
#define STRICT_TYPED_ITEMIDS
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>
#include <shobjidl.h>  // For IFileDialogEvents

#include "serialize/iseriazable.h"
#include <random> // pour std::mt19937 et std::uniform_real_distribution

#include "rendering/render_system.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;



Editor::Editor()
{
	PROFILER_NOOP;

}

Editor::~Editor()
{
}

void Editor::InitThridPartLib(PC_CORE::GraphicAPI graphicApi)
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	PC_LOG("InitThridPartLib...")
	ShaderSource::InitShadersCompiler(graphicApi, false);

}

void Editor::UnInitThridPartLib()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	ShaderSource::DestroyShadersCompiler();
}

void Editor::CompileShader()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	PC_LOG("CompileShader...")
	std::filesystem::create_directory(SHADER_CACHE_PATH);
	
	auto forwardVert = ResourceManager::Create<ShaderSource>("forward.vert");
	forwardVert->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/forward/forward.vert");

	auto forwardFrag = ResourceManager::Create<ShaderSource>("forward.frag");
	forwardFrag->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/forward/forward.frag");

	auto screenQuadVert = ResourceManager::Create<ShaderSource>("draw_texture_screen_quad.vert");
	screenQuadVert->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/draw_texture_screen_quad.vert");

	auto screenQuadFrag = ResourceManager::Create<ShaderSource>("draw_texture_screen_quad.frag");
	screenQuadFrag->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/draw_texture_screen_quad.frag");

	auto skyRenderingVert = ResourceManager::Create<ShaderSource>("sky_rendering.vert");
	skyRenderingVert->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/sky_rendering.vert");

	auto skyRenderingFrag = ResourceManager::Create<ShaderSource>("sky_rendering.frag");
	skyRenderingFrag->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/sky_rendering.frag");

	auto debugDrawVert = ResourceManager::Create<ShaderSource>("debug_draw.vert");
	debugDrawVert->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/debug_draw/debug_draw.vert");
		
	auto debugDrawFrag = ResourceManager::Create<ShaderSource>("debug_draw.frag");
	debugDrawFrag->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/debug_draw/debug_draw.frag");

	auto debugDrawRayVert = ResourceManager::Create<ShaderSource>("debug_draw_ray.vert");
	debugDrawRayVert->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/debug_draw/debug_draw_ray.vert");

	// skybox
	{
		auto skyboxVert = ResourceManager::Create<ShaderSource>("cube_map_skybox.vert");
		skyboxVert->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/cube_map_skybox/cube_map_skybox.vert");

		auto skyboxFrag = ResourceManager::Create<ShaderSource>("cube_map_skybox.frag");
		skyboxFrag->LoadFromFile(EDITOR_RESOURCE_PATH "/shaders/cube_map_skybox/cube_map_skybox.frag");
	}

	// sprite
	{
		auto spriteVert = ResourceManager::Create<ShaderSource>("draw_sprite.vert"
			, EDITOR_RESOURCE_PATH "/shaders/draw_sprite/draw_sprite.vert");

		auto spriteFrag = ResourceManager::Create<ShaderSource>("draw_sprite.frag",
			EDITOR_RESOURCE_PATH "/shaders/draw_sprite/draw_sprite.frag");
	}

	// geometry buffer
	{
		auto geometryVert = ResourceManager::Create<ShaderSource>("geometry.vert",
		EDITOR_RESOURCE_PATH "/shaders/geometry/geometry.vert");
		
		auto geometryFrag = ResourceManager::Create<ShaderSource>("geometry.frag",
		EDITOR_RESOURCE_PATH "/shaders/geometry/geometry.frag");
	}

	// Tone Map
	{
		auto toneMap = ResourceManager::Create<ShaderSource>("aces.comp",
		EDITOR_RESOURCE_PATH "/shaders/post_process/tone_mapping/aces.comp");
	}

	// deferred
	{
		auto deferredFrag = ResourceManager::Create<ShaderSource>("deferred.frag",
		EDITOR_RESOURCE_PATH "/shaders/deferred/deferred.frag");
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
		.appLogoPath = EDITOR_RESOURCE_PATH "/logo/paraconquer_logo_black.png",
		.enableGpuDebug = true,
		.graphicAPI = GraphicAPI::Vulkan
	};
	
	InitThridPartLib(appCreateInfo.graphicAPI);
	CompileShader();
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

	UnInitThridPartLib();
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
			  
			PC_CORE::ResourceManager::ForEach(PC_CORE::Reflector::GetTypeKey<PC_CORE::ShaderSource>(), l);
			
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
	
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);
	PC_LOG("InitTestScene...")

	std::shared_ptr<Material> m1 = ResourceManager::Create<Material>("diamond_block_material.mat");
	std::shared_ptr<Material> m2 = ResourceManager::Create<Material>("emerauld_block_material.mat");

	m1->albedo = ResourceManager::Get<Texture2D>("diamond_block.jpg");
	m1->Build();


	m2->albedo = ResourceManager::Get<Texture2D>("emerauld_block.png");
	m2->Build();

	auto& level = World::GetWorld()->level;
	


	EntityId sphere = level.CreateEntity("sphere");
	level.AddComponent<Transform>(sphere);
	level.AddComponent<StaticMesh>(sphere);
	Transform* t = &level.GetComponent<Transform>(sphere);
	t->position = Tbx::Vector3d(0.0f, 0.0f, 0.0f);
	t->scale = Tbx::Vector3d(2.0f, 2.0f, 2.0f);

	
	StaticMesh* mesh2 = &level.GetComponent<StaticMesh>(sphere);
	mesh2->mesh = ResourceManager::Get<Mesh>("sphere.obj");
	mesh2->material = m2;


	EntityId pointLight = level.CreateEntity("pointLight");
	level.AddComponent<Transform>(pointLight);
	level.AddComponent<PointLight>(pointLight);
	t = &level.GetComponent<Transform>(pointLight);
	t->position = Tbx::Vector3d(0.0f, 2.5f, 0.0f);
	t->scale = Tbx::Vector3d(1.0f, 1.0f, 1.0f);

	PointLight& p = level.GetComponent<PointLight>(pointLight);
	p.intensity = 5.f;
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
	editorWindows.push_back(std::make_unique<AssetBrowser>(*this, "AssetBrowser"));

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
