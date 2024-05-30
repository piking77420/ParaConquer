#include "shader_source.h"

#include "log.hpp"

using namespace PC_CORE;

void ShaderSource::Load(const fs::path& path)
{
    uint32_t formatIndex = -1;
    
    if (!IResource::IsFormatValid(ShaderSourceFormat, path, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }

    name = path.filename().stem().generic_string();
    format = format.at(formatIndex);
    
}
