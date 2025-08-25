#include "AssetBrowser.hpp"

#include <Imgui/imgui_impl_vulkan.h>
#include "Editor.hpp"
#include "EditorFormat.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/VulkanSampler.hpp"
#include "World/World.hpp"


#include <Fstream>
#include <ImguiHelper.h>

using namespace PC_EDITOR_CORE;


AssetBrowser::AssetBrowser(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    constexpr const char* projectBaseAssetPath = "assets";
    m_BasePath = std::filesystem::path(projectBaseAssetPath);
    m_CurrenPath = m_BasePath;
    instance = this;

    ReloadOldAssets();

    for (size_t i = 0; i < m_AssetBrowserTexture.size(); i++)
    {
        const AssetsBrowserTexturesType assetsBrowserTexturesType = static_cast<AssetsBrowserTexturesType>(i);
        switch (assetsBrowserTexturesType)
        {
        case PC_EDITOR_CORE::AssetBrowser::AssetsBrowserTexturesType::Folder:
            m_AssetBrowserTexture[i].texure = PC_CORE::Texture2D("folder.png", EDITOR_RESOURCE_PATH "/icons/folder.png");
            break;
        case PC_EDITOR_CORE::AssetBrowser::AssetsBrowserTexturesType::Cout:
        default:
            assert(false);
            break;
        }

        m_Editor->IMGUIContext.CreateImguiVulkanTexture(&m_AssetBrowserTexture[i].texure, &m_AssetBrowserTexture[i].descritproSet, 1);
    }

    const FileWatcherEvents fileWatcherEvents =
    {
        std::bind(&AssetBrowser::OnFileModify, this, std::placeholders::_1),
		nullptr,
		nullptr,
		nullptr
    };

    const FileWatcherCreateInfo fileWatcherCreateInfo =
    {
        .watcherName = "ShaderWatcher",
        .watchRoot = EDITOR_RESOURCE_PATH"/Shaders",
        .expectFormat =
        {
            L".hlsl"
        },
        .fileWatcherEvents = fileWatcherEvents,
    };

    m_FileWatcherShaders.LauchWatcher(fileWatcherCreateInfo);
}

void AssetBrowser::ReloadOldAssets()
{
    // TO DOES THIS TO FILE SYSTEM WATCHER
    // READ project resource files
    // read each sub file in project dir 
    // if name match in project resource files
    // Add to project with old guid else load resoirce
}

AssetBrowser::~AssetBrowser()
{
    for (size_t i = 0; i < m_AssetBrowserTexture.size(); i++)
    {
        m_Editor->IMGUIContext.DestroyVulkanTexture(&m_AssetBrowserTexture[i].descritproSet, 1);
    }
}


void AssetBrowser::Render()
{
    EditorWindow::Render();
}

void PC_EDITOR_CORE::AssetBrowser::Update()
{
    PERF_REGION_SCOPED;
    EditorWindow::Update();

    if (!IsCursorInsideWindow() || ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        //m_SelectedItem = "";
    }

    if (IsCursorInsideWindow() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup("CreateAsset", 0);

    if (ImGui::BeginPopup("CreateAsset"))
    {
        CreateAsset();
        ImGui::EndPopup();
    }

    RenderDirectories();

    if (!m_HasSelectedObject && 
        ImGui::IsWindowFocused() && IsCursorInsideWindow() 
        && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        m_Editor->selectedObject = std::monostate();
        m_HasSelectedObject = false;
    }
}

void AssetBrowser::TriggerReload(const std::filesystem::path& _path)
{

}


void AssetBrowser::CreateAsset() const
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup("createAssets");

    
    if (ImGui::BeginPopup("createAssets"))
    {
        ImGui::SeparatorText("Assets");

        if (ImGui::Selectable("Level"))
        {
            auto s = GetUniqueFileName(m_CurrenPath, "Level", std::string(PC_Level));
            CreateFile((m_CurrenPath / s).string());
        }

        ImGui::EndPopup();
    }
    
}

void AssetBrowser::RenderDirectories()
{
    PERF_REGION_SCOPED;

    float columnSpacing = 100;
    float padding = 16.f;
    float thumbailSize = 64;
    float cellsize = thumbailSize + padding;

    float panelwidht = ImGui::GetContentRegionAvail().x;
    int colomnCount = (int)(panelwidht / cellsize);
    if (colomnCount < 1)
        colomnCount = 1;
    
    if (m_CurrenPath == m_BasePath)
    {

    }
    else if (ImGui::ArrowButton("Reverse", ImGuiDir_Left) && m_BasePath.string() != m_CurrenPath.string())
    {
        m_CurrenPath = m_CurrenPath.parent_path();
    }

    ImGui::Columns(colomnCount, 0, false);

    for (auto& entry : std::filesystem::directory_iterator(m_CurrenPath))
    {
        const auto& path = entry.path();
        auto relative = path.relative_path();
        std::string name = relative.filename().string();
        
        if (entry.is_directory())
        {
           
            const AssetsBrowserTextures& icon = m_AssetBrowserTexture[(int)AssetsBrowserTexturesType::Folder];

            ImGui::Image((ImTextureID)icon.descritproSet, { thumbailSize, thumbailSize }, { 0, 0 }, { 1, 1 });
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_HasSelectedObject = true;
                m_CurrenPath = entry;
            }   
            ImGui::Text(name.c_str());
        }
        else
        {
            if (ImGui::Button(name.c_str(), { thumbailSize,thumbailSize }))
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



        ImGui::NextColumn();
    }

    ImGui::Columns(1);
}

void AssetBrowser::CreateFile(const std::string& _filename) const
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


std::string AssetBrowser::GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const
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

void AssetBrowser::OnFileSelectedClick()
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

void AssetBrowser::OnFileModify(const FileModifyEventData& _fileModifyName)
{
    std::lock_guard _(lock);

}
