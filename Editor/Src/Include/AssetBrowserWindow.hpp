
#include <Filesystem>
#include <Vulkan/Vulkan.h>

#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "Asset.hpp"
#include "Resources/Resource.hpp"
#include "FileSystemWatcher.h"
#include "Resources/Texture2d.hpp"
#include "Io/ImguiContext.h"

#include "Singleton.hpp"
#include "Rendering/Sampler.hpp"
#include "Math/ToolboxHeaderfile.hpp"

BEGIN_EDITOR_PCCORE


constexpr Tbx::Vector2f MIN_MAX_FILE_SPACING = Tbx::Vector2f(1.f, 1000.f);

class AssetBrowserWindow : public EditorWindow
{
public:
    void Render() override;

    void Update() override;

    void TriggerReload(const std::filesystem::path& _path);

    AssetBrowserWindow(Editor& _editor, const std::string& _name);

    ~AssetBrowserWindow() override;
private:

    struct AssetBrowserOption
    {
        float spacing = 50.f;
        float padding = 16.f;
    }m_AssetBrowserOption{};


    ImguiImage m_FolderIcon;

    ImguiImage m_NullIcon;

    std::filesystem::path m_CurrenPath;

    std::filesystem::path m_SelectedItem;

    std::unordered_map<std::string, ImguiImage> m_FormatIconMap;

    std::unordered_map<std::filesystem::path, std::string> m_PathStringCache;

    bool m_HasSelectedObject = false;

    void CreateAsset() const;

    void RenderDirectories();

    void CreateFile(const std::string& _filename) const;

    std::string GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const;

    void OnFileSelectedClick();

    void CreateAssetsBrowserIcon(const char* _format, const std::filesystem::path& _path);
};

END_EDITOR_PCCORE