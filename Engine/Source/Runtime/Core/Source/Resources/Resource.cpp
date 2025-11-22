#include "resources/resource.hpp"

#include <Fstream>
#include <Utility>

#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;

#include <Filesystem>


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

Resource& Resource::operator=(const Resource& _other) noexcept
{
    DynamicReflectable::operator=(_other);
    Name = _other.Name;
    m_Guid = Guid::New();
    m_ChildsResource = _other.m_ChildsResource;
    m_ParentsResource = _other.m_ParentsResource;

    return *this;
}

Resource& Resource::operator=(Resource&& _other) noexcept
{
    DynamicReflectable::operator=(_other);
    Name = std::move(_other.Name);
    m_Guid = Guid::New();
    m_ChildsResource = std::move(_other.m_ChildsResource);
    m_ParentsResource = std::move(_other.m_ParentsResource);

    return *this;
}


Resource::Resource(const Resource& _other) noexcept : DynamicReflectable(_other),
                                                      Name(_other.Name),
                                                      m_Guid(Guid::New()), m_ParentsResource(_other.m_ParentsResource),
                                                      m_ChildsResource(_other.m_ChildsResource)
{
}

Resource::Resource(Resource&& _other) noexcept
{
    DynamicReflectable::operator=(_other);
    Name = std::move(_other.Name);
    m_Guid = _other.m_Guid;
    m_ChildsResource = std::move(_other.m_ChildsResource);
    m_ParentsResource = std::move(_other.m_ParentsResource);
}

Resource::Resource(const std::string& _name) : Name(_name), m_Guid(Guid::New())
{
}

Resource::Resource(std::string&& _name) : Name(std::move(_name)), m_Guid(Guid::New())
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
