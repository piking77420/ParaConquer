#include <thread> 

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
void Editor::InitThridPartLib()
{
	std::cout << "InitThridPartLib" << std::endl;
	PERF_REGION_SCOPED;
	glslang_initialize_process();

}

void Editor::UnInitThridPartLib()
{
	PERF_REGION_SCOPED;
	glslang_finalize_process();
}

void Editor::CompileShader()
{
	std::cout << "CompileShader" << std::endl;

	PERF_REGION_SCOPED;

	std::shared_ptr<ShaderSource> vertex = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH"shaders/main.vert");
	std::shared_ptr<ShaderSource> frag = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH"shaders/main.frag");

	vertex->CompileToSpriv();
	frag->CompileToSpriv();

	std::cout << "CompileShader 2" << std::endl;

	vertex = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "shaders/draw_texture_screen_quad.vert");
	frag = ResourceManager::Create<ShaderSource>(EDITOR_RESOURCE_PATH "shaders/draw_texture_screen_quad.frag");


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


void Editor::Init()
{
	PERF_REGION_SCOPED;
	LookForEditorInit();


	InitThridPartLib();
	CompileShader();

	const AppCreateInfo appCreateInfo =
	{
		EDITOR_RESOURCE_PATH "logo/paraconquer_logo_black.png"
	};

	gameApp.Init(appCreateInfo);

	IMGUIContext.Init(gameApp.window.GetHandle(), Rhi::GetInstance().GetGraphicsAPI());

	gameApp.renderer.primaryCommandList->RecordFetchCommand([&](CommandList* cmd) {
		IMGUIContext.Render(cmd);
		});

	InitTestScene();
	InitEditorWindows();
}

void Editor::Destroy()
{
	PERF_REGION_SCOPED;

	IMGUIContext.Destroy();

	for (auto& i: m_EditorWindows)
		i.release();

	gameApp.Destroy();

	UnInitThridPartLib();
}
void Editor::UpdateEditorWindows()
{
	//static bool open = true;

	//ImGui::ShowDemoWindow(&open);

	dockSpace.BeginDockSpace();
	if (ImGui::BeginMenuBar())
	{
	;

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("SaveScene"))
			{
				auto world = World::GetWorld();
				// unordoredMap Function
				using UnordoredByteMap = std::unordered_map<uint8_t, uint8_t>;
				using UnordoredMapIterator = typename UnordoredByteMap::iterator;
				using ReseverMapFunction = void (UnordoredByteMap::*)(size_t);
				using InsertMapFunction = typename UnordoredByteMap::mapped_type& (UnordoredByteMap::*)(const typename UnordoredByteMap::key_type&);

				using UnordoredMapConstIterator = typename UnordoredByteMap::const_iterator;
				using UnorderedMapUnrefConstIteratorFunc = const std::pair<const typename UnordoredByteMap::key_type, typename UnordoredByteMap::mapped_type>* (UnordoredMapConstIterator::*)() const;
				using IncrementMapIterator = UnordoredMapConstIterator & (UnordoredMapConstIterator::*)();
				using IncrementMapIteratorByte = std::unordered_map<uint8_t, uint8_t>::const_iterator& (std::unordered_map<uint8_t, uint8_t>::const_iterator::*)();

#if 0

				void* ptr = &world->m_ComponentManager.m_ComponentMapArray[Reflector::GetTypeKey<RigidBody>()];
				std::unordered_map<uint8_t, uint8_t>* mapByte = reinterpret_cast<std::unordered_map<uint8_t, uint8_t>*>(&world->m_ComponentManager.m_ComponentMapArray);

				const auto& typeMap = Reflector::GetType<std::unordered_map<TypeId, ComponentArray>>();

				UnorderedMapUnrefConstIteratorFunc unrefFunf = nullptr;
				auto& mapfuns = Reflector::m_UnordoredMapReflectFunction.at(typeMap.typeId);
				std::memcpy(&unrefFunf, &mapfuns.unrefFunc, sizeof(UnorderedMapUnrefConstIteratorFunc));

				//auto rBegin = world->m_ComponentManager.m_ComponentMapArray.begin();
				//auto rEnd = world->m_ComponentManager.m_ComponentMapArray.end();

				

				IncrementMapIterator incrementFunc = &std::unordered_map<uint8_t, uint8_t>::const_iterator::operator++;
				IncrementMapIteratorByte iteratorFunByte;
				memcpy(&iteratorFunByte, &incrementFunc, sizeof(IncrementMapIteratorByte));

				// TO DO REFLECT THE STD PAIR OF MAP TO GET THE OFFET BETWEEN THOS TWOO VALUE
				for (UnordoredMapConstIterator it = mapByte->begin();
					it != mapByte->end();)
				{
					const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&(it.*unrefFunf)()->first);
					const uint8_t* ptr2 = reinterpret_cast<const uint8_t*>((&(it.*unrefFunf)()->first) + typeMap.metaData.typeNatureMetaData.metaDataType.unordoredMapReflected.offsetBetweenKeyAndValueInPair);
				
					const auto& type = Reflector::GetType(*reinterpret_cast<const uint32_t*>(ptr));


					const ComponentArray* arry = reinterpret_cast<const ComponentArray*>(ptr2);
					assert(type.typeId == arry->componentType);

					printf("");
					(it.*iteratorFunByte)();
				}

				Serializer::Serialize(world->m_ComponentManager.m_ComponentMapArray, "basic_level.level");
#else

				
				auto& map = ResourceManager::m_ResourcesMap;
				const auto& typeMap = Reflector::GetType<decltype(ResourceManager::m_ResourcesMap)>();
				std::unordered_map<uint8_t, uint8_t>* mapByte = reinterpret_cast<std::unordered_map<uint8_t, uint8_t>*>(&ResourceManager::m_ResourcesMap);

				UnorderedMapUnrefConstIteratorFunc unrefFunf = nullptr;
				auto& mapfuns = Reflector::m_UnordoredMapReflectFunction.at(typeMap.typeId);
				std::memcpy(&unrefFunf, &mapfuns.unrefFunc, sizeof(UnorderedMapUnrefConstIteratorFunc));

				IncrementMapIterator incrementFunc = &std::unordered_map<uint8_t, uint8_t>::const_iterator::operator++;
				IncrementMapIteratorByte iteratorFunByte;
				memcpy(&iteratorFunByte, &incrementFunc, sizeof(IncrementMapIteratorByte));


				for (auto it = mapByte->begin() ; it != mapByte->end(); )
				{
					void* ptrr = &ResourceManager::m_ResourcesMap.at("quad.obj");

					constexpr size_t sizeOfS = sizeof(std::string);
					const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&(it.*unrefFunf)()->first);
					const uint8_t* ptr2 = reinterpret_cast<const uint8_t*>((&(it.*unrefFunf)()->first) + typeMap.metaData.typeNatureMetaData.metaDataType.unordoredMapReflected.offsetBetweenKeyAndValueInPair);

					const std::string* s = reinterpret_cast<const std::string*>(ptr);
					const std::shared_ptr<Resource>* r = reinterpret_cast<const std::shared_ptr<Resource>*>(ptr2);


					(it.*iteratorFunByte)();
				}
#endif

			}
			ImGui::EndMenu();

		}
		ImGui::EndMenuBar();

	}

	for (auto& editorWindow : m_EditorWindows)
	{
		editorWindow->Begin();
		editorWindow->Update();
		editorWindow->End();
	}


	dockSpace.EndDockSpace();
}


REFLECT(std::vector<Transform>)

std::shared_ptr<Material> m1;
std::shared_ptr<Material> m2;

void Editor::InitTestScene()
{
	PERF_REGION_SCOPED;

	m1 = std::make_shared<Material>("diamond_block_material.mat");
	m2 = std::make_shared<Material>("emerauld_block_material.mat");

	m1->m_albedo = ResourceManager::Get<Texture>("diamond_block.jpg");
	m1->Build();


	m2->m_albedo = ResourceManager::Get<Texture>("emerauld_block.png");
	m2->Build();


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
	t->position = Tbx::Vector3d(0.0f, 2.0f, 1.0f );

	StaticMesh* mesh = &World::GetWorld()->GetComponent<StaticMesh>(cube);
	mesh->mesh = ResourceManager::Get<Mesh>("cube.obj");
	mesh->material = m1;

	StaticMesh* mesh2 = &World::GetWorld()->GetComponent<StaticMesh>(sphere);
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
		IMGUIContext.NewFrame();
		PC_CORE::Time::UpdateTime();

		if (!gameApp.renderer.BeginDraw(&gameApp.window))
			continue;
		
		UpdateEditorWindows();
		gameApp.WorldTick();

		for (auto& editorWindow : m_EditorWindows)
			editorWindow->Render();
		
		gameApp.renderer.SwapBuffers(&gameApp.window);
		PERF_FRAME_MARK;
	}

	Rhi::GetRhiContext()->WaitIdle();
}

void Editor::InitEditorWindows()
{
	m_EditorWindows.push_back(std::make_unique<EditWorldWindow>(*this, "Scene"));
	m_EditorWindows.push_back(std::make_unique<Inspector>(*this, "Inspector"));
	m_EditorWindows.push_back(std::make_unique<Hierachy>(*this, "Hierachy"));
	m_EditorWindows.push_back(std::make_unique<SceneButton>(*this, "SceneButton"));
	m_EditorWindows.push_back(std::make_unique<AssetBrowser>(*this, "AssetBrowser"));
}
