#include "resources/resource.hpp"

using namespace PC_CORE;

IResource::IResource() : guid(Guid::New())
{
    
}

IResource::IResource(std::string _name, std::string _format, fs::path _path, Guid _guid) : name(std::move(_name)),
format(std::move(_format)), path(std::move(_path)), guid(_guid)
{
}
