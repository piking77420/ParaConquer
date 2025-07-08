#include "resources/resource.hpp"

#include <fstream>
#include <utility>

#include "resources/resource_manager.hpp"

using namespace PC_CORE;


void Resource::BroadCastReload()
{
	/*
	auto childs = ResourceManager::GetChildResource(name);
	if  (childs == nullptr)
		return;

	for (auto& child : *childs)
	{
		
		if (auto ptr = child.lock())
		{
			ptr->OnParentReload(name);
		}
	}*/
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
	
	return *this;
}

Resource& Resource::operator=(Resource&& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = std::move(_other.name);
	extension = std::move(_other.extension);
	m_Guid = Guid::New();
	
	return *this;
}


Resource::Resource(const Resource& _other) noexcept : ISeriazable(_other), 
name(_other.name), extension(_other.extension),
m_Guid(Guid::New())
{
	
}

Resource::Resource(Resource&& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = std::move(_other.name);
	extension = std::move(_other.extension);
	m_Guid = _other.m_Guid;
}

Resource::Resource(const std::string& _name) : name(_name) , m_Guid(Guid::New())
{
	
}

Resource::Resource(std::string&& _name) : name(std::move(_name)) , m_Guid(Guid::New())
{
	
}

Resource::Resource(const fs::path& _file)
{
	fs::path pathFileName = _file.filename();
	name = pathFileName.generic_string();	
	extension = pathFileName.extension().generic_string();
}


