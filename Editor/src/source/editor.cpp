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
#include <Imgui/imgui_internal.h>

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;

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

struct dqdq
{
	TypeId ds;

	ComponentArray componentArray;
};

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

	for (EditorWindow* editorWindow : m_EditorWindows)
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
