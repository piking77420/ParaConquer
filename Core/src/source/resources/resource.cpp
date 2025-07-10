#include "resources/resource.hpp"

#include <fstream>
#include <utility>

#include "resources/resource_manager.hpp"

using namespace PC_CORE;

#include <filesystem>
void Resource::LoadFromFile(const std::string& _path)
{
	std::filesystem::path pathFileName = std::filesystem::path(_path).filename();
	name = pathFileName.generic_string();
	extension = pathFileName.extension().generic_string();
}

void Resource::LinkDependencies(Resource* _resourceParent,  Resource* _resourceChild)
{
	_resourceParent->m_ChildsResource.emplace_back(_resourceChild->GetGuid());
	_resourceChild->m_ParentsResource.emplace_back(_resourceParent->GetGuid());
}

void Resource::BroadCastReload()
{
	for (auto& child : m_ChildsResource)
	{
#ifdef WITH_EDITOR && _DEBUG
		if (!ResourceManager::Exist(child))
		{
			// TODO FORMAT GUID
			PC_LOGERROR("Invalid child guid, parent = {}, child guid = {}", name);
		}
#endif

		auto r = ResourceManager::Get<Resource>(child);
		r->OnParentReload(GetGuid());
		r->BroadCastReload();
	}
}

const std::atomic<bool>& Resource::IsLoaded() const
{
	return m_IsLoaded;
}

Resource& Resource::operator=(const Resource& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = _other.name;
	extension = _other.extension;
	m_Guid = Guid::New();
	m_ChildsResource = _other.m_ChildsResource;
	m_ParentsResource = _other.m_ParentsResource;
	
	return *this;
}

Resource& Resource::operator=(Resource&& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = std::move(_other.name);
	extension = std::move(_other.extension);
	m_Guid = Guid::New();
	m_ChildsResource = std::move(_other.m_ChildsResource);
	m_ParentsResource = std::move(_other.m_ParentsResource);
	
	return *this;
}


Resource::Resource(const Resource& _other) noexcept : ISeriazable(_other), 
name(_other.name), extension(_other.extension),
m_Guid(Guid::New()), m_ChildsResource(_other.m_ChildsResource), m_ParentsResource(_other.m_ParentsResource)
{
	
}

Resource::Resource(Resource&& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = std::move(_other.name);
	extension = std::move(_other.extension);
	m_Guid = _other.m_Guid;
	m_ChildsResource = std::move(_other.m_ChildsResource);
	m_ParentsResource = std::move(_other.m_ParentsResource);
}

Resource::Resource(const std::string& _name) : name(_name) , m_Guid(Guid::New())
{
	
}

Resource::Resource(std::string&& _name) : name(std::move(_name)) , m_Guid(Guid::New())
{
	
}


