#pragma once

#include <filesystem>

#include "EditorHeader.hpp"
#include "Asset.hpp"
#include "FileSystemWatcher.h"

BEGIN_EDITOR_PCCORE

struct AssetBrowserData
{
    std::unordered_map<PC_CORE::Guid, Asset> m_Assets;
    std::unordered_map<std::filesystem::path, PC_CORE::Guid> m_PathToAssetsGuid; 


    REFLECT(AssetBrowserData)
    REFLECT_MEMBER(AssetBrowserData, m_Assets)
    REFLECT_MEMBER(AssetBrowserData, m_PathToAssetsGuid)

};

class AssetBrowser : private AssetBrowserData
{
public:

    std::mutex lock;

    void SetPath(const std::filesystem::path& _rootPath);

    inline const std::filesystem::path& GetBasePath() const
    {
        return m_BasePath;
    }

    inline static AssetBrowser& GetInstance()
    {
        return *m_Instance;
    }

    bool Exist(const std::filesystem::path& _path) const;

    bool Exist(const PC_CORE::Guid& _guid) const;

    const Asset& CreateAsset(const std::filesystem::path& _path);

    const Asset& CreateAsset(const std::filesystem::path& _path, const PC_CORE::Guid& guid);

    bool Delete(const std::filesystem::path& _path);

    const Asset* GetAsset(const std::filesystem::path& _path) const;

    const Asset* GetAsset(const PC_CORE::Guid& _guid) const;

    AssetBrowser();

    ~AssetBrowser();
private:

  
    static inline AssetBrowser* m_Instance = nullptr;

    std::filesystem::path m_BasePath;

    FileSystemWatcher m_FileWatcherShaders;


   
    void CreateFile(const std::string& _filename) const;
    
    std::string GetUniqueFileName(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) const;

    void OnFileSelectedClick();

    void OnFileModify(const FileModifyEventData& _fileModifyName);

    void IsDirectoryHasBeenAdded(const std::filesystem::path& _path);

     Asset* GetAsset(const std::filesystem::path& _path) ;

     Asset* GetAsset(const PC_CORE::Guid& _guid) ;


    REFLECT(AssetBrowser, AssetBrowserData);
    REFLECT_MEMBER(AssetBrowser, m_Assets);
    REFLECT_MEMBER(AssetBrowser, m_PathToAssetsGuid);


};

END_EDITOR_PCCORE