#include <thread> 
#include <chrono>
#include <iostream>


#include <perf_region.hpp>
#include <glslang/Include/glslang_c_interface.h>
#include <Imgui/imgui_internal.h>
#include <Json/json.hpp>


#include "editor.hpp"
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

#include "editor_sub_system/debug_draw_context.hpp"
#include "serialize/iseriazable.h"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;



Editor::Editor()
{
	PROFILER_NOOP;

}

Editor::~Editor()
{
}


namespace ImGui {

	//https://github.com/ocornut/imgui/issues/1901
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	}

	bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);
	}

}
void Editor::InitThridPartLib(PC_CORE::GraphicAPI graphicApi)
{
	PERF_REGION_SCOPED;

	PC_LOG("InitThridPartLib...")
	ShaderSource::InitShadersCompiler(graphicApi, false);

}

void Editor::UnInitThridPartLib()
{
	PERF_REGION_SCOPED;
	ShaderSource::DestroyShadersCompiler();
}

void Editor::CompileShader()
{
	PERF_REGION_SCOPED;
	PC_LOG("CompileShader...")
	fs::create_directory(SHADER_CACHE_PATH);
	
	std::shared_ptr<ShaderSource> vertex = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH"/shaders/main.vert");
	std::shared_ptr<ShaderSource> frag = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH"/shaders/main.frag");

	vertex->CompileToSpriv();
	frag->CompileToSpriv();

	vertex = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "/shaders/draw_texture_screen_quad.vert");
	frag = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "/shaders/draw_texture_screen_quad.frag");
	
	vertex->CompileToSpriv();
	frag->CompileToSpriv();

	vertex = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "/shaders/sky_rendering.vert");
	frag = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "/shaders/sky_rendering.frag");
	
	vertex->CompileToSpriv();
	frag->CompileToSpriv();

	vertex = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "/shaders/debug_draw/debug_draw.vert");
	frag = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "/shaders/debug_draw/debug_draw.frag");
	
	vertex->CompileToSpriv();
	frag->CompileToSpriv();
}

void Editor::LookForEditorInit()
{
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
	}
	
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

void Editor::ReloadShaders()
{
	
}

void Editor::InitSubSystem()
{
	editorSubSystems.push_back(std::make_unique<DebugDrawContext>(*this));
}


void Editor::Init()
{
	PERF_REGION_SCOPED;
	
	const AppCreateInfo appCreateInfo =
	{
		.appName = "Para Conquer Editor",
		.appLogoPath = EDITOR_RESOURCE_PATH "/logo/paraconquer_logo_black.png",
		.enableGpuDebug = true,
		.graphicAPI = GraphicAPI::VULKAN
	};
	
	InitThridPartLib(appCreateInfo.graphicAPI);
	CompileShader();
	gameApp.Init(appCreateInfo);
	
	IMGUIContext.Init(gameApp.window.GetHandle(), Rhi::GetInstance().GetGraphicsAPI());
	
	
	gameApp.renderer.primaryCommandList->RecordFetchCommand([&](CommandList* cmd) {
		cmd->BeginDebugLabel("Imgui Draw", IMGUI_RENDER_DEBUG_COLOR);
		IMGUIContext.Render(cmd);
		cmd->EndDebugLabel();
		});
	
	InitTestScene();
	InitEditorWindows();
}

void Editor::Destroy()
{
	PERF_REGION_SCOPED;

	IMGUIContext.Destroy();

	for (auto& i : editorWindows)
	{
		i.reset();
	}

	gameApp.Destroy();

	UnInitThridPartLib();
}
void Editor::UpdateEditorWindows()
{
	static bool open = true;

	ImGui::ShowDemoWindow(&open);

	dockSpace.BeginDockSpace();
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("SaveScene"))
			{
				Level& l = World::GetWorld()->level;
				Serializer::Serialize(l,"TestScene.map");
			}
			if (ImGui::MenuItem("LoadScene"))
			{
				Level& l = World::GetWorld()->level;
				Serializer::DeSerialize(&l,"TestScene.map");
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Rendering"))
		{
			auto l = [&](std::shared_ptr<Resource> _shader)
			{
				if (ImGui::MenuItem(_shader->name.c_str()))
				{

					// reload shader
				}
			};
			
			PC_CORE::ResourceManager::ForEach(PC_CORE::Reflector::GetTypeKey<PC_CORE::ShaderProgram>(), l);
			
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	for (auto& editorWindow : editorWindows)
	{
		editorWindow->Begin();
		editorWindow->Update();
		editorWindow->End();
	}

	EditorCommandUpdate();
	dockSpace.EndDockSpace();
}


std::shared_ptr<Material> m1;
std::shared_ptr<Material> m2;

void Editor::RewindCommand()
{
	if (editorCommands.empty())
		return;

	editorCommands.pop_back();
}

void Editor::InitTestScene()
{	
	
	PERF_REGION_SCOPED;
	PC_LOG("InitTestScene...")

	m1 = ResourceManager::Create<Material>("diamond_block_material.mat");
	m2 = ResourceManager::Create<Material>("emerauld_block_material.mat");

	m1->m_albedo = ResourceManager::Get<Texture2D>("diamond_block.jpg");
	m1->Build();


	m2->m_albedo = ResourceManager::Get<Texture2D>("emerauld_block.png");
	m2->Build();

	auto& level = World::GetWorld()->level;

	EntityId dirLight = level.CreateEntity("dirLight");
	level.AddComponent<DirLight>(dirLight);
	level.AddComponent<Transform>(dirLight);
	Transform* t = &level.GetComponent<Transform>(dirLight);
	t->rotation = Rotation(Tbx::Vector3f::UnitY());
	DirLight* dir = &level.GetComponent<DirLight>(dirLight);
	dir->color = Tbx::Vector3f(1, 1, 1);

	EntityId cube = level.CreateEntity("cube");
	level.AddComponent<Transform>(cube);
	level.AddComponent<StaticMesh>(cube);
	t = &level.GetComponent<Transform>(cube);
	t->position = Tbx::Vector3d(5.0f, 5.0f, 1.0f);

	
	EntityId sphere = level.CreateEntity("sphere");
	level.AddComponent<Transform>(sphere);
	level.AddComponent<StaticMesh>(sphere);
	t = &level.GetComponent<Transform>(sphere);
	t->position = Tbx::Vector3d(0.0f, 0.0f, 0.0f);
	t->scale = Tbx::Vector3d(10.0f, 10.0f, 10.0f);

	StaticMesh* mesh = &level.GetComponent<StaticMesh>(cube);
	mesh->mesh = ResourceManager::Get<Mesh>("rounded_cube.obj");
	mesh->material = m1;

	
	StaticMesh* mesh2 = &level.GetComponent<StaticMesh>(sphere);
	mesh2->mesh = ResourceManager::Get<Mesh>("sphere.obj");
	mesh2->material = m2;

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
		PERF_REGION_SCOPED;

		gameApp.coreIo.PoolEvent();
		gameApp.window.PoolEvents();
		PC_CORE::Time::UpdateTime();
		IMGUIContext.NewFrame();

		gameApp.renderer.BeginDraw(&gameApp.window);
		
		UpdateEditorWindows();
		gameApp.WorldTick();

		for (auto& editorWindow : editorWindows)
			editorWindow->Render();

		gameApp.renderer.SwapBuffers(&gameApp.window);
		PERF_FRAME_MARK;

		
	}

	Rhi::GetRhiContext()->WaitIdle();
}

void Editor::InitEditorWindows()
{
	PC_LOG("InitEditorWindows...")


	editorWindows.push_back(std::make_unique<EditWorldWindow>(*this, "Scene"));
	editorWindows.push_back(std::make_unique<Inspector>(*this, "Inspector"));
	editorWindows.push_back(std::make_unique<Hierachy>(*this, "Hierachy"));
	editorWindows.push_back(std::make_unique<SceneButton>(*this, "SceneButton"));
	editorWindows.push_back(std::make_unique<AssetBrowser>(*this, "AssetBrowser"));
}

void Editor::EditorCommandUpdate()
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
	{
		RewindCommand();
	}
}
