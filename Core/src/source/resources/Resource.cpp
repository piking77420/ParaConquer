#include "resources/Resource.hpp"

using namespace PC_CORE;

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

