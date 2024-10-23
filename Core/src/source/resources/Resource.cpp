#include "resources/Resource.hpp"

using namespace PC_CORE;

Resource::Resource() : guid(Guid::New())
{
    
}

Resource::Resource(std::string _name, std::string _format, fs::path _path, Guid _guid) : name(std::move(_name)),
format(std::move(_format)), path(std::move(_path)), guid(_guid)
{
}
