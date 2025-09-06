#include <thread> 
#include <Chrono>
#include <Iostream>


#include <PerfRegion.hpp>
#include <Imgui/imgui_internal.h>


#include "Editor.hpp"
#include "Resources/ResourceManager.hpp"


#include "ResourceBrowserWindow.hpp"
#include "EditWorldWindow.hpp"
#include "Hierachy.hpp"
#include "Inspector.hpp"
#include "SceneButton.hpp"
#include "WorldViewWindow.hpp"
#include "Time/CoreTime.hpp"
#include <Resources/ResourceManager.hpp>

#include "EditorFiles.hpp"
#include "ProjectMaker.hpp"
#include "SystemDialogue.hpp"
#include "Rendering/Light.hpp"
#include "Io/CoreIo.hpp"
#include "Io/ImguiContext.h"
#include "Physics/RigidBody.hpp"
#include "Rendering/Material.hpp"
#include "Resources/ShaderSource.hpp"
#include "World/StaticMeshComponent.hpp"
#include "Serialize/Serializer.h"
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
	editorData.projectData.graphicApi = GraphicAPI::Vulkan;
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
		Serializer::DeSerialize(&editorIniFile, std::string(EditorIniFileName)); // copy it 

		if (std::filesystem::exists(editorIniFile.projectPath)) // if editor.ini is valid
		{
			Serializer::DeSerialize(&projectFile, editorIniFile.projectPath + "/" + std::string(ProjectFileName)); // copy project 
		}
		else
		{
			const std::wstring sw = std::wstring(editorIniFile.projectPath.begin(), editorIniFile.projectPath.end());
			projectFile = ProjectMaker::CreateBaseProject(sw.c_str());
		}
	}
	else
	{
		const std::wstring s = SystemDialogue::Instance().SeletecFolder(L"Select your project folder");
		assert(!s.empty() && "Something went wrong");

		if (!std::filesystem::exists(s + std::wstring(ProjectFileName.begin(), ProjectFileName.end())))
		{
			projectFile = ProjectMaker::CreateBaseProject(s.c_str());
		}
		
		editorIniFile.projectPath = std::string(s.begin(), s.end());	
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
	
	Serializer::Serialize<EditorIniFile>(editorIniFile, std::string(EditorIniFileName));
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
			EDITOR_RESOURCE_PATH "/Shaders/Geometry/Geometry.vs.hlsl");

		auto geometryFrag = ResourceManager::Create<ShaderSource>("Geometry.ps.hlsl",
			EDITOR_RESOURCE_PATH "/Shaders/Geometry/Geometry.ps.hlsl");
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
		auto toneMap = ResourceManager::Create<ShaderSource>("Aces.cs.hlsl",
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

void Editor::Init()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	LoadFromInitFiles();
	
	const AppCreateInfo appCreateInfo =
	{
		.appName = editorData.projectData.projectName,
		.appLogoPath = EDITOR_RESOURCE_PATH "/logo/ParaConquerLogoBlack.png",
		.enableGpuDebug = true,
		.graphicAPI = editorData.projectData.graphicApi
	};

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
	ImGui::PopFont();
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
	// to do move this 
	editorData.nearestSampler.~Sampler();
}

void Editor::InitEditor()
{
	PERF_REGION_SCOPED;
	PERF_REGION_COLOR(PerfRegion::Editor);

	{
		PC_LOG("Init Editor NearestSampler...")

		const PC_CORE::SamplerCreateInfo info =
		{
		.SamplerName = "ImguiImageSampler",
		.magFilter = PC_CORE::Filter::LINEAR,
		.minFilter = PC_CORE::Filter::LINEAR,
		.u = PC_CORE::SamplerAddressMode::REPEAT,
		.v = PC_CORE::SamplerAddressMode::REPEAT,
		.w = PC_CORE::SamplerAddressMode::REPEAT
		};

		editorData.nearestSampler = PC_CORE::Sampler(info);

	}


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
