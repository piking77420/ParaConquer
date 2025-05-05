#include "resources/resource.hpp"

#include <fstream>
#include <utility>

using namespace PC_CORE;

Resource::Resource(Resource&& _other) noexcept : name(std::move(_other.name)),
extension(std::move(_other.extension)),
guid(std::move(_other.guid)) 
{

}

Resource::Resource()
{

}

Resource::Resource(const fs::path& _file)
{
	fs::path pathFileName = _file.filename();
	name = pathFileName.generic_string();
	extension = pathFileName.extension().generic_string();
}

