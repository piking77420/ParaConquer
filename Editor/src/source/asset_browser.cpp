#include "asset_browser.hpp"

using namespace PC_EDITOR_CORE;


AssetBrowser::AssetBrowser(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    m_BasePath = std::filesystem::path("assets");
    m_CurrenPath = m_BasePath;
}

void PC_EDITOR_CORE::AssetBrowser::Update()
{
    EditorWindow::Update();
    RenderBrowser();
}

void AssetBrowser::RenderBrowser()
{
    return;
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

    for (auto& entry : fs::directory_iterator(m_CurrenPath))
    {
        const auto& path = entry.path();
        auto relative = path.relative_path();
        std::string name = relative.filename().string();
        
        if (entry.is_directory())
        {

            //ImGui::ImageButton((ImTextureID)FolderIcon->ID, { thumbailSize,thumbailSize }, { 0,1 }, { 1,0 });

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_CurrenPath = path;
            }
            ImGui::Text(name.c_str());
        }
        else
        {
            if (ImGui::Button(name.c_str(), { thumbailSize,thumbailSize }))
            {
                std::string ressourcename = path.filename().generic_string();
              
            }
            ImGui::Text(name.c_str());
        }
        ImGui::NextColumn();

    }

    ImGui::Columns(1);

}
