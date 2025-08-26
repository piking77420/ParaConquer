#include "AssetBrowser.hpp"

#include "Serialize/Serializer.h"

#include <fstream>

using namespace PC_EDITOR_CORE;

void AssetBrowser::SetPath(const std::filesystem::path& _rootPath)
{
    m_FileWatcherShaders.Stop();
    m_BasePath = std::filesystem::path(_rootPath);
 
    std::lock_guard _(lock);

    // recusrive init
    IsDirectoryHasBeenAdded(m_BasePath);


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

bool AssetBrowser::Exist(const PC_CORE::Guid& _guid) const
{
    return m_Assets.contains(_guid);
}

bool AssetBrowser::Exist(const std::filesystem::path& _path) const
{
    return m_PathToAssetsGuid.contains(_path);
}

const Asset& AssetBrowser::CreateAsset(const std::filesystem::path& _path)
{
    return CreateAsset(_path, PC_CORE::Guid::New());
}

const Asset& AssetBrowser::CreateAsset(const std::filesystem::path& _path, const PC_CORE::Guid& guid)
{
    m_PathToAssetsGuid[_path] = guid;
    m_Assets[guid] = Asset(_path, guid);

    return m_Assets[guid];
}

bool AssetBrowser::Delete(const std::filesystem::path& _path)
{
    if (!Exist(_path))
    {
        PC_LOGERROR("There is no asset name as with path {}", _path.generic_string());
        return false;
    }

    const PC_CORE::Guid id = m_PathToAssetsGuid[_path];
    m_PathToAssetsGuid.erase(_path);

    assert(m_Assets.contains(id) && "This should be always valid");
    m_Assets.erase(id);
    return true;
}

const Asset* AssetBrowser::GetAsset(const std::filesystem::path& _path) const
{
    if (!Exist(_path))
    {
        PC_LOGERROR("There is no asset with path {}", _path.generic_string());
        return nullptr;
    }

    return &m_Assets.at(m_PathToAssetsGuid.at(_path));
}

const Asset* AssetBrowser::GetAsset(const PC_CORE::Guid& _guid) const
{
    if (!Exist(_guid))
    {
        PC_LOGERROR("There is no asset name as with Guid {}", (std::string)_guid);
        return nullptr;
    }

    return &m_Assets.at(_guid);
}

Asset* AssetBrowser::GetAsset(const std::filesystem::path& _path)
{
    if (!Exist(_path))
    {
        PC_LOGERROR("There is no asset with path{}", _path.generic_string());
        return nullptr;
    }

    return &m_Assets.at(m_PathToAssetsGuid.at(_path));
}

Asset* AssetBrowser::GetAsset(const PC_CORE::Guid& _guid)
{
    if (!Exist(_guid))
    {
        PC_LOGERROR("There is no asset name as with Guid {}", (std::string)_guid);
        return nullptr;
    }

    return &m_Assets[_guid];
}


AssetBrowser::AssetBrowser()
{
    assert(m_Instance == nullptr);
    m_Instance = this;
    AssetBrowserData* data = this;
    PC_CORE::Serializer::DeSerialize<AssetBrowserData>(data, "AssetBrowset.data");
}

AssetBrowser::~AssetBrowser()
{
    const AssetBrowserData& data = *this;
    PC_CORE::Serializer::Serialize<AssetBrowserData>(data, "AssetBrowset.data");
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

void AssetBrowser::OnFileModify(const FileModifyEventData& _fileModifyName)
{

}

void AssetBrowser::IsDirectoryHasBeenAdded(const std::filesystem::path& _path)
{
    for (auto& entry : std::filesystem::directory_iterator(_path))
    {
        if (entry.is_directory())
        {
            IsDirectoryHasBeenAdded(entry);
        }
        else
        {
            if (!m_PathToAssetsGuid.contains(entry.path()))
            {
                CreateAsset(entry);
            }
            else
            {
                Asset* asset = GetAsset(entry);
                asset->UpdateIfPathChanged(entry.path());
            }
        }


    }

}

