#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

enum class LowLevelShaderType 
        {
            VERTEX,
            FRAGMENT,
            GEOMETRY,
            TESSELATION,
            COMPUTE,

            SHADERTYPEMAX
        };

        constexpr const char* ShaderSourceFormat[5]
        {
            ".vert",
            ".frag",
            ".geom",
            ".tess",
            ".comp"
          };

struct ShaderSourceCode
{
    uint8_t* Data;
    uint32_t Size;
};

struct ShaderSourceAndPath
{
    ShaderSourceCode shaderSourceCode;
    const char* shaderSourceCodePath;
};
        

END_PCCORE