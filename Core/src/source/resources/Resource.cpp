#include "resources/Resource.hpp"

#include <utility>

using namespace PC_CORE;

Resource::Resource(Resource&& _other) noexcept : name(std::move(_other.name)),
format(std::move(_other.format)), 
path(std::move(_other.path)),
guid(std::move(_other.guid)) 
{

}

Resource::Resource() : guid(Guid::New())
{

}

Resource::Resource(const fs::path& _path)
{
	SetPath(_path);
	const fs::path& fileName = _path.filename();
	name = fileName.generic_string();
	format = fileName.extension().generic_string();
	guid = Guid::New();
}

void Resource::SetPath(const fs::path& _path)
{
	path = _path;
}

const std::string& Resource::GetName() const
{
	return name;
}

