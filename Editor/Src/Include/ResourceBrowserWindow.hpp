#pragma once

#include <chrono>
#include <Filesystem>
#include <Vulkan/Vulkan.h>

#include "Guid.hpp"

#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "Resources/Resource.hpp"
#include "FileSystemWatcher.h"
#include "Resources/Texture2d.hpp"
#include "Io/ImguiContext.h"

#include "Singleton.hpp"
#include "Rendering/Sampler.hpp"
#include "Math/ToolboxHeaderfile.hpp"

BEGIN_EDITOR_PCCORE


constexpr Tbx::Vector2f MIN_MAX_FILE_SPACING = Tbx::Vector2f(1.f, 1000.f);

class ResourceBrowserWindow : public EditorWindow
{
public:
    void Render() override;

    void Update() override;

    void TriggerReload(const std::filesystem::path& _path);

    ResourceBrowserWindow(Editor& _editor, const std::string& _name);

    ~ResourceBrowserWindow() override;
private:

    struct AssetBrowserOption
    {
        float spacing = 50.f;
        float padding = 16.f;
        float thumbnailSize = 48.f;
        
    }m_AssetBrowserOption{};

    struct AssetFile
    {
        PC_CORE::TypeId typeId;
        PC_CORE::Guid assetGuid;
        std::time_t lastTimeModified;

        DEFAULT_COPY_MOVE_OPERATIONS(AssetFile);

        explicit AssetFile(const PC_CORE::Resource& _r) :
            typeId(_r.GetType().typeId), assetGuid(_r.GetGuid()),
            lastTimeModified(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
        {

        };

        explicit AssetFile(PC_CORE::TypeId _typeId, PC_CORE::Guid _assetGuid, std::time_t _lastTimeModified) :
            typeId(_typeId), assetGuid(_assetGuid),
            lastTimeModified(_lastTimeModified)
        {

        };

        explicit AssetFile() = default;

        ~AssetFile() = default;

        REFLECT(AssetFile);
        REFLECT_MEMBER(AssetFile, typeId);
        REFLECT_MEMBER(AssetFile, assetGuid);
        REFLECT_MEMBER(AssetFile, lastTimeModified);
    };

    struct AssetRegistery
    {
        std::unordered_map<std::filesystem::path, AssetFile> pathToType;
        REFLECT(AssetRegistery);
        REFLECT_MEMBER(AssetRegistery, pathToType);

    }m_AssetRegistery;
   
    ImguiImage m_FolderIcon;

    ImguiImage m_NullIcon;

    std::filesystem::path m_BasePathRelative;

    std::filesystem::path m_CurrenPath;

    std::filesystem::path m_SelectedItem;

    std::unordered_map<PC_CORE::TypeId, ImguiImage> m_TypeIconMap;

    bool m_HasSelectedObject = false;

    std::string GetAssetRegisterPath() const;

    std::time_t GetLastTimeModifyFile(const std::filesystem::path& _p) const;

    void CreateAsset() const;

    void RenderDirectories();

    void CreateFile(const std::string& _filename) const;

    std::string GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const;

    void CreateAssetsBrowserIcon(PC_CORE::TypeId _id, const std::filesystem::path& _path);

    PC_CORE::TypeId TypeIdFromPath(const std::filesystem::path& _path);

    //Import 
    void OnImportButton();
};

END_EDITOR_PCCORE