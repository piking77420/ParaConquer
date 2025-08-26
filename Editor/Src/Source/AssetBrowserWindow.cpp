#include "AssetBrowserWindow.hpp"

#include <Imgui/imgui_impl_vulkan.h>
#include "Editor.hpp"
#include "EditorFormat.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/VulkanSampler.hpp"
#include "World/World.hpp"


#include <Fstream>
#include <ImguiHelper.h>

using namespace PC_EDITOR_CORE;


AssetBrowserWindow::AssetBrowserWindow(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
	m_CurrenPath = AssetBrowser::GetInstance().GetBasePath();
	windowFlags |= ImGuiWindowFlags_MenuBar;

	const PC_CORE::Sampler& s = m_Editor->editorData.nearestSampler;

	m_FolderIcon.texure = PC_CORE::Texture2D("Folder.png", EDITOR_RESOURCE_PATH "/Icons/Folder.png");
	m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_FolderIcon.texure.GetRhiTexture2D().get(),s.GetRhiSampler().get(), &m_FolderIcon.descritproSet, 1);


	m_NullIcon.texure = PC_CORE::Texture2D("Null.png", EDITOR_RESOURCE_PATH "/Icons/Null.png");
	m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_NullIcon.texure.GetRhiTexture2D().get(),s.GetRhiSampler().get(), &m_NullIcon.descritproSet, 1);

	CreateAssetsBrowserIcon(".png", EDITOR_RESOURCE_PATH "/Icons/PngIcon.png");
	CreateAssetsBrowserIcon(".jpg", EDITOR_RESOURCE_PATH "/Icons/JpgIcon.png");
	CreateAssetsBrowserIcon(".png", EDITOR_RESOURCE_PATH "/Icons/PngIcon.png");
	CreateAssetsBrowserIcon(".dds", EDITOR_RESOURCE_PATH "/Icons/DdsIcon.png");

	CreateAssetsBrowserIcon(".gltg", EDITOR_RESOURCE_PATH "/Icons/GltfIcon.png");
	CreateAssetsBrowserIcon(".obj", EDITOR_RESOURCE_PATH "/Icons/ObjIcon.png");
	CreateAssetsBrowserIcon(".fbx", EDITOR_RESOURCE_PATH "/Icons/FbxIncon.png");

}

AssetBrowserWindow::~AssetBrowserWindow()
{
	m_Editor->IMGUIContext.DestroyVulkanTexture(&m_FolderIcon.descritproSet, 1);
	m_Editor->IMGUIContext.DestroyVulkanTexture(&m_NullIcon.descritproSet, 1);

	for (auto& it : m_FormatIconMap)
		m_Editor->IMGUIContext.DestroyVulkanTexture(&it.second.descritproSet, 1);

}


void AssetBrowserWindow::Render()
{
	EditorWindow::Render();
}

void PC_EDITOR_CORE::AssetBrowserWindow::Update()
{
	PERF_REGION_SCOPED;
	EditorWindow::Update();

	// lock
	std::lock_guard _(AssetBrowser::GetInstance().lock);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Option"))
		{
			ImGui::Text("Asset Browser Settings");
			ImGui::Separator();

			ImGui::DragFloat("Spacing", &m_AssetBrowserOption.spacing, 1.0f, MIN_MAX_FILE_SPACING.x, MIN_MAX_FILE_SPACING.y, "%.1f px");
			ImGui::DragFloat("Padding", &m_AssetBrowserOption.padding, 1.0f, 10.0f, 40.0f, "%.1f px");

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (IsCursorInsideWindow() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("CreateAsset", 0);

	if (ImGui::BeginPopup("CreateAsset"))
	{
		CreateAsset();
		ImGui::EndPopup();
	}

	ImGui::PushFont(m_Editor->editorData.editorFont.veryBig);
	ImGui::Text(m_CurrenPath.generic_string().c_str());
	ImGui::PopFont();
	RenderDirectories();

	if (!m_HasSelectedObject &&
		ImGui::IsWindowFocused() && IsCursorInsideWindow()
		&& ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		m_Editor->selectedObject = std::monostate();
		m_HasSelectedObject = false;
	}
}

void AssetBrowserWindow::CreateAsset() const
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("createAssets");


	if (ImGui::BeginPopup("createAssets"))
	{
		ImGui::SeparatorText("Assets");

		//if (ImGui::Selectable("Textures"))

		if (ImGui::Selectable("Level"))
		{
			auto s = GetUniqueFileName(m_CurrenPath, "Level", std::string(PC_Level));
			CreateFile((m_CurrenPath / s).string());
		}

		ImGui::EndPopup();
	}

}

void AssetBrowserWindow::RenderDirectories()
{
	PERF_REGION_SCOPED;
	ImGui::PushFont(m_Editor->editorData.editorFont.normal);


	const float columnSpacing = m_AssetBrowserOption.spacing;
	const float padding = m_AssetBrowserOption.padding;
	const float thumbnailSize = 64.0f;

	float cellSize = thumbnailSize + padding * 2.0f + columnSpacing;

	float panelwidht = ImGui::GetContentRegionAvail().x;
	int colomnCount = (int)(panelwidht / cellSize);
	if (colomnCount < 1)
		colomnCount = 1;

	if (m_CurrenPath == AssetBrowser::GetInstance().GetBasePath())
	{

	}
	else if (ImGui::ArrowButton("Reverse", ImGuiDir_Left) && AssetBrowser::GetInstance().GetBasePath().string() != m_CurrenPath.string())
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
				m_HasSelectedObject = true;
				m_CurrenPath = entry;
			}
		}
		else if (pathname.has_extension())
		{
			auto it = m_FormatIconMap.find(pathname.extension().generic_string());

			auto& icon = it == m_FormatIconMap.end() ? m_NullIcon : it->second;

			if (ImGui::ImageButton(name.c_str(), (ImTextureID)icon.descritproSet, { thumbnailSize,thumbnailSize }))
			{
				if (m_SelectedItem == entry)
				{
					m_HasSelectedObject = true;
					OnFileSelectedClick();
				}

				if (m_SelectedItem.empty())
				{
					m_SelectedItem = entry;
					PC_LOG("File selected: {}", name);
				}

			}

		}


		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
		ImGui::TextWrapped(name.c_str());
		ImGui::PopTextWrapPos();

		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	ImGui::PopFont();
}

void AssetBrowserWindow::CreateFile(const std::string& _filename) const
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


std::string AssetBrowserWindow::GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const
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

void AssetBrowserWindow::OnFileSelectedClick()
{

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

	m_SelectedItem = fileName;
}

void AssetBrowserWindow::CreateAssetsBrowserIcon(const char* _format, const std::filesystem::path& _path)
{
	ImguiImage newIcon;
	newIcon.texure = PC_CORE::Texture2D(_path.filename().generic_string(), _path.generic_string());
	m_Editor->IMGUIContext.CreateImguiVulkanTexture(
		newIcon.texure.GetRhiTexture2D().get(), 
		m_Editor->editorData.nearestSampler.GetRhiSampler().get(), 
		&newIcon.descritproSet, 1);

	m_FormatIconMap[std::string(_format)] = std::move(newIcon);
}
