#pragma once

#include <Filesystem>
#include <Vulkan/Vulkan.h>

#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "CoreHeader.hpp"
#include "Resources/Resource.hpp"
#include "FileSystemWatcher.h"
#include "Resources/Texture2d.hpp"
#include "Singleton.hpp"

BEGIN_EDITOR_PCCORE


class AssetBrowser : public EditorWindow
{
public:
    AssetBrowser* instance = nullptr;

    void Render() override;
    
    void Update() override;

    void TriggerReload(const std::filesystem::path& _path);
    
    AssetBrowser(Editor& _editor, const std::string& _name);

    ~AssetBrowser() override;
private:

	enum struct AssetsBrowserTexturesType
	{
		Folder,
		//Texture, TODO
	 	Cout
	};

    struct AssetsBrowserTextures
    {
        VkDescriptorSet descritproSet;
        PC_CORE::Texture2D texure;
    };

    std::mutex lock;

    std::array<AssetsBrowserTextures, (size_t)AssetsBrowserTexturesType::Cout> m_AssetBrowserTexture;

    std::filesystem::path m_BasePath;

    std::filesystem::path m_CurrenPath;

    std::filesystem::path m_SelectedItem;

    FileSystemWatcher m_FileWatcherShaders;

    bool m_HasSelectedObject = false;

    std::unordered_map<PC_CORE::Guid, std::vector<std::filesystem::path>> m_ResourceGuidToPath;

    std::unordered_map<std::filesystem::path, PC_CORE::Guid> m_PathToGuid;

    void ReloadOldAssets();

    void CreateAsset() const;

    void RenderDirectories();
    
    void CreateFile(const std::string& _filename) const;
    
    std::string GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const;

    void OnFileSelectedClick();

    void OnFileModify(const FileModifyEventData& _fileModifyName);

};

END_EDITOR_PCCORE