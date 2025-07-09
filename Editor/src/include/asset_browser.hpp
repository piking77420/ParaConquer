#pragma once
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "core_header.hpp"
#include "resources/resource.hpp"
#include "file_system_watcher.h"
#include "resources/texture_2d.hpp"

BEGIN_EDITOR_PCCORE
    class AssetBrowser : public EditorWindow
{
public:
    void Render() override;
    
    void Update() override;

    AssetBrowser(Editor& _editor, const std::string& _name);
    
    ~AssetBrowser() override = default  ;
private:

    PC_CORE::Texture2D m_FolderTexture;

    //fs::path m_BasePath;

    //fs::path m_CurrenPath;

    //fs::path m_SelectedItem;

    FileSystemWatcher m_fileWatcher;

    void CreateAsset() const;

    void RenderDirectories();
    
    void CreateFile(const std::string& _filename) const;
    
    //std::string GetUniqueFileName(const fs::path& directory, const std::string& baseName, const std::string& extension) const;

    void OnFileSelectedClick();

};

END_EDITOR_PCCORE