#include "ResourceBrowserWindow.hpp"

#include <Imgui/imgui_impl_vulkan.h>
#include "Editor.hpp"
#include "EditorFiles.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/VulkanSampler.hpp"
#include "World/World.hpp"
#include "SystemDialogue.hpp"


#include <Fstream>
#include <ImguiHelper.h>

using namespace PC_EDITOR_CORE;


ResourceBrowserWindow::ResourceBrowserWindow(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
	m_BasePathRelative = std::filesystem::relative(std::filesystem::current_path(), m_Editor->editorData.projectPath);
	m_CurrenPath = m_Editor->editorData.projectPath;

	windowFlags |= ImGuiWindowFlags_MenuBar;

	const PC_CORE::Sampler& s = m_Editor->editorData.nearestSampler;

	m_FolderIcon.texure = PC_CORE::Texture2D("Folder.png", EDITOR_RESOURCE_PATH "/Icons/Folder.png");
	m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_FolderIcon.texure.GetRhiTexture2D().get(),s.GetRhiSampler().get(), &m_FolderIcon.descritproSet, 1);


	m_NullIcon.texure = PC_CORE::Texture2D("Null.png", EDITOR_RESOURCE_PATH "/Icons/Null.png");
	m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_NullIcon.texure.GetRhiTexture2D().get(),s.GetRhiSampler().get(), &m_NullIcon.descritproSet, 1);

	CreateAssetsBrowserIcon(PC_CORE::Reflector::GetTypeKey<PC_CORE::Texture2D>(), EDITOR_RESOURCE_PATH "/Icons/PngIcon.png");
	CreateAssetsBrowserIcon(PC_CORE::Reflector::GetTypeKey<PC_CORE::StaticMesh>(), EDITOR_RESOURCE_PATH "/Icons/3DModel.png");

}

ResourceBrowserWindow::~ResourceBrowserWindow()
{
	m_Editor->IMGUIContext.DestroyVulkanTexture(&m_FolderIcon.descritproSet, 1);
	m_Editor->IMGUIContext.DestroyVulkanTexture(&m_NullIcon.descritproSet, 1);

	for (auto& it : m_TypeIconMap)
		m_Editor->IMGUIContext.DestroyVulkanTexture(&it.second.descritproSet, 1);

}


void ResourceBrowserWindow::Render()
{
	EditorWindow::Render();
}

void PC_EDITOR_CORE::ResourceBrowserWindow::Update()
{
	PERF_REGION_SCOPED;
	EditorWindow::Update();
	

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Option"))
		{
			ImGui::Text("Asset Browser Settings");
			ImGui::Separator();

			ImGui::DragFloat("Spacing", &m_AssetBrowserOption.spacing, 1.0f, MIN_MAX_FILE_SPACING.x, MIN_MAX_FILE_SPACING.y, "%.1f px");
			ImGui::DragFloat("Padding", &m_AssetBrowserOption.padding, 1.0f, 10.0f, 40.0f, "%.1f px");
			ImGui::DragFloat("ThumbnailSize", &m_AssetBrowserOption.thumbnailSize, 1.0f, 10.0f, 100.0f, "%.1f px");

			ImGui::EndMenu();
		}

		if (ImGui::Button("Import"))
		{
			const std::wstring& assetPath = m_Editor->editorData.projectPath;
			const std::wstring s = SystemDialogue::Instance().SeletecFile(L"Select your project imported source", assetPath.c_str());

			// TODO make importer

		}

		ImGui::EndMenuBar();
	}

	
	m_HasSelectedObject = false;

	ImGui::PushFont(m_Editor->editorData.editorFont.veryBig);
	ImGui::Text(m_BasePathRelative.generic_string().c_str());
	ImGui::PopFont();
	RenderDirectories();

	if (ImGui::IsWindowFocused() && IsCursorInsideWindow())
	{

		if (!m_HasSelectedObject && ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			CreateAsset();
		}

		if (!m_HasSelectedObject && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_Editor->selectedObject = std::monostate();
		}
	}

}

void ResourceBrowserWindow::CreateAsset() const
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("createAssets");


	if (ImGui::BeginPopup("CreateAssets"))
	{
		ImGui::SeparatorText("Assets");

		//if (ImGui::Selectable("Textures"))

		ImGui::BeginPopup("Textures");
		if (ImGui::Selectable("Texture2D"))
		{
			const PC_CORE::SamplerCreateInfo info =
			{
			.SamplerName = "LinearRepeat",
			.magFilter = PC_CORE::Filter::LINEAR,
			.minFilter = PC_CORE::Filter::LINEAR,
			.u = PC_CORE::SamplerAddressMode::REPEAT,
			.v = PC_CORE::SamplerAddressMode::REPEAT,
			.w = PC_CORE::SamplerAddressMode::REPEAT
			};

			//PC_CORE::Sampler newTexture(GetUniqueFileName(m_CurrenPath, "Texture2D", ".presource"));
			
		}



		ImGui::EndPopup();

		if (ImGui::Selectable("Level"))
		{
			//auto s = GetUniqueFileName(m_CurrenPath, "Level", std::string(PC_Level));
			//CreateFile((m_CurrenPath / s).string());
		}

		ImGui::EndPopup();
	}

}

void ResourceBrowserWindow::RenderDirectories()
{
	PERF_REGION_SCOPED;
	ImGui::PushFont(m_Editor->editorData.editorFont.normal);


	const float columnSpacing = m_AssetBrowserOption.spacing;
	const float padding = m_AssetBrowserOption.padding;
	const float thumbnailSize = m_AssetBrowserOption.thumbnailSize;

	float cellSize = thumbnailSize + padding * 2.0f + columnSpacing;

	float panelwidht = ImGui::GetContentRegionAvail().x;
	int colomnCount = (int)(panelwidht / cellSize);
	if (colomnCount < 1)
		colomnCount = 1;

	if (m_CurrenPath == m_Editor->editorData.projectPath)
	{

	}
	else if (ImGui::ArrowButton("Reverse", ImGuiDir_Left) && m_Editor->editorData.projectPath.string() != m_CurrenPath.string())
	{
		m_CurrenPath = m_CurrenPath.parent_path();
	}

	ImGui::Columns(colomnCount, 0, false);

	for (auto& entry : std::filesystem::directory_iterator(m_CurrenPath))
	{
		const auto& path = entry.path();
		std::filesystem::path pathname = path.filename();

		if (!m_PathStringCache.contains(path))
		{
			m_PathStringCache[path] = pathname.generic_string();
		}

		const std::string& name = m_PathStringCache[path];

		// Compute Text Size
		ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
		float scale = thumbnailSize / textSize.x;

		if (entry.is_directory())
		{
			const ImguiImage& icon = m_FolderIcon;

			ImGui::Image((ImTextureID)icon.descritproSet, { thumbnailSize, thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_CurrenPath = entry;
			}

			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
			ImGui::TextWrapped(name.c_str());
			ImGui::PopTextWrapPos();
			ImGui::NextColumn();
		}
		else if (pathname.has_extension() && pathname.extension() == AssetsFormat)
		{
			auto type = TypeIdFromPath(path); // TODO MAKE A THREAD THAT HANDLE THIS 

			if (type != PC_CORE::NullTypeId && PC_CORE::Reflector::ContaintTypeFromTypeID(type))
			{
				auto it = m_TypeIconMap.find(type);

				auto& icon = it == m_TypeIconMap.end() ? m_NullIcon : it->second;

				if (ImGui::ImageButton(name.c_str(), (ImTextureID)icon.descritproSet, { thumbnailSize,thumbnailSize }))
				{
					if (m_SelectedItem == entry)
					{
						OnFileSelectedClick();
					}

					if (m_SelectedItem.empty())
					{
						m_SelectedItem = entry;
						PC_LOG("File selected: {}", name);
					}

					ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
					ImGui::TextWrapped(name.c_str());
					ImGui::PopTextWrapPos();
					ImGui::NextColumn();

				}
			}

			

		}


		
	}

	ImGui::Columns(1);
	ImGui::PopFont();
}

void ResourceBrowserWindow::CreateFile(const std::string& _filename) const
{
	std::ofstream file(_filename);

	if (file.is_open())
	{
		file.close();
		PC_LOG("File created: {}", _filename);
	}
	else
	{
		PC_LOG("Failed to created: {}", _filename);
	}
}


std::string ResourceBrowserWindow::GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const
{
	std::string fileName = baseName + extension;
	int counter = 1;

	while (std::filesystem::exists(directory / fileName))
	{
		fileName = baseName + "_" + std::to_string(counter) + extension;
		++counter;
	}

	return fileName;
}

void ResourceBrowserWindow::OnFileSelectedClick()
{
/*
	if (m_SelectedItem.empty())
	{
		PC_LOGERROR("m_SelectedItem is empty")
			return;
	}


	std::string fileName = m_SelectedItem.string();
	PC_LOG("File clicked: {}", fileName);

	std::string fileFormat = m_SelectedItem.extension().string();

	if (fileFormat == PC_Level)
	{
		PC_LOG("Load level selected: {}", fileName);
	}

	m_SelectedItem = fileName;*/
}

void ResourceBrowserWindow::CreateAssetsBrowserIcon(PC_CORE::TypeId _id, const std::filesystem::path& _path)
{
	ImguiImage newIcon;
	newIcon.texure = PC_CORE::Texture2D(_path.filename().generic_string(), _path.generic_string());
	m_Editor->IMGUIContext.CreateImguiVulkanTexture(
		newIcon.texure.GetRhiTexture2D().get(), 
		m_Editor->editorData.nearestSampler.GetRhiSampler().get(), 
		&newIcon.descritproSet, 1);

	m_TypeIconMap[_id] = std::move(newIcon);
}

PC_CORE::TypeId ResourceBrowserWindow::TypeIdFromPath(const std::filesystem::path& _path)
{
	PERF_REGION_SCOPED;

	json j;
	std::ifstream f(_path);

	if (!f.is_open())
	{
		f.close();
		PC_LOGERROR("Failed to open file {}", _path.generic_string());
		return PC_CORE::NullTypeId;
	}

	{

		j = json::parse(f);
		f.close();
		if (!j.contains(PC_CORE::Serializer::RESOURCE_TYPE))
		{
			return PC_CORE::NullTypeId;
		}
	}

	return j[PC_CORE::Serializer::RESOURCE_TYPE];
}
