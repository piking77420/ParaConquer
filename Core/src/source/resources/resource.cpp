#include "resources/resource.hpp"

#include <fstream>
#include <utility>

using namespace PC_CORE;

const std::atomic<bool>& Resource::IsLoaded() const
{
	return m_IsLoaded;
}

Resource& Resource::operator=(const Resource& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = _other.name;
	extension = _other.extension;
	guid = Guid::New();
	pathToFile = _other.pathToFile;
	
	return *this;
}

Resource& Resource::operator=(Resource&& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = std::move(_other.name);
	extension = std::move(_other.extension);
	guid = Guid::New();
	pathToFile = std::move(_other.pathToFile);
	
	return *this;
}


Resource::Resource(const Resource& _other) noexcept : ISeriazable(_other), 
name(_other.name), extension(_other.extension),
guid(Guid::New()), pathToFile(_other.pathToFile) 
{
	
}

Resource::Resource(Resource&& _other) noexcept
{
	ISeriazable::operator=(_other);
	name = std::move(_other.name);
	extension = std::move(_other.extension);
	guid = _other.guid;
	pathToFile = std::move(_other.pathToFile);
}

Resource::Resource(const std::string& _name) : name(_name)
{
	
}

Resource::Resource(const fs::path& _file)
{
	fs::path pathFileName = _file.filename();
	name = pathFileName.generic_string();	
	extension = pathFileName.extension().generic_string();
}

