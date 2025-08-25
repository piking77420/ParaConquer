
#include <Filesystem>
#include <Vulkan/Vulkan.h>

#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "Asset.hpp"
#include "Resources/Resource.hpp"
#include "FileSystemWatcher.h"
#include "Resources/Texture2d.hpp"

#include "Singleton.hpp"
#include "Rendering/Sampler.hpp"

BEGIN_EDITOR_PCCORE


class AssetBrowserWindow : public EditorWindow
{
public:
    void Render() override;

    void Update() override;

    void TriggerReload(const std::filesystem::path& _path);

    AssetBrowserWindow(Editor& _editor, const std::string& _name);

    ~AssetBrowserWindow() override;
private:
    struct AssetsBrowserIcons
    {
        VkDescriptorSet descritproSet;
        PC_CORE::Texture2D texure;
    };

    std::mutex lock;

    PC_CORE::Sampler m_ImageSampler;

    AssetsBrowserIcons m_FolderIcons;

    AssetsBrowserIcons m_TextureIcons;


    std::filesystem::path m_CurrenPath;

    std::filesystem::path m_SelectedItem;

    bool m_HasSelectedObject = false;

    void CreateAsset() const;

    void RenderDirectories();

    void CreateFile(const std::string& _filename) const;

    std::string GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const;

    void OnFileSelectedClick();

    const AssetsBrowserIcons& GetIcon();
};

END_EDITOR_PCCORE