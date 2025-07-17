#pragma once

#include <filesystem>
#include <vulkan/vulkan.h>

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
    
    ~AssetBrowser() override;
private:

	enum struct AssetsBrowserTexturesType
	{
		Folder,
		Texture,
		Cout
	};

    struct AssetsBrowserTextures
    {
        VkDescriptorSet descritproSet;
        PC_CORE::Texture2D texure;
    };

    std::array<AssetsBrowserTextures, (size_t)AssetsBrowserTexturesType::Cout> m_AssetBrowserTexture;

    std::filesystem::path m_BasePath;

    std::filesystem::path m_CurrenPath;

    std::filesystem::path m_SelectedItem;

    FileSystemWatcher m_fileWatcher;

    bool m_HasSelectedObject = false;

    void ReloadOldAssets();

    void CreateAsset() const;

    void RenderDirectories();
    
    void CreateFile(const std::string& _filename) const;
    
    std::string GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const;

    void OnFileSelectedClick();

};

END_EDITOR_PCCORE