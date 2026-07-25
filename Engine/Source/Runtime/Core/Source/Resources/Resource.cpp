#include <Resources/Resource.hpp>

#include <fstream>
#include <utility>

#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;

#include <filesystem>


void Resource::LinkDependencies(Resource* _resourceParent, Resource* _resourceChild)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    _resourceParent->m_ChildsResource.emplace_back(_resourceChild->GetGuid());
    _resourceChild->m_ParentsResource.emplace_back(_resourceParent->GetGuid());
}

void Resource::BroadCastReload()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    for (auto& child : m_ChildsResource)
    {
#ifdef WITH_EDITOR && _DEBUG
        if (!ResourceManager::Exist(child))
        {
            // TODO FORMAT GUID
            PC_LOGERROR("Invalid child guid, parent = {}, child guid = {}", Name);
        }
#endif

        auto r = ResourceManager::Get<Resource>(child);
        r->OnParentReload(GetGuid());
        r->BroadCastReload();
    }
}

Resource::Resource(const std::string& _name)
    : Name(_name)
{
}

Resource::Resource(std::string&& _name)
    : Name(std::move(_name))
{
}

std::string Resource::GetFullExtension(const std::string& _path)
{
    std::string filename = std::filesystem::path(_path).filename().string();
    size_t firstDot = filename.find('.'); // first dot in filename
    if (firstDot != std::string::npos)
    {
        return filename.substr(firstDot); // from first dot to end
    }
    return ""; // no extension
}
