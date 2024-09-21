#pragma once


#include <string>

#include "core_header.hpp"
#include <vector>
#include <array>
BEGIN_PCCORE

enum class LowLevelShaderStageType 
{
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    TESSELATION,
    COMPUTE,

    COUNT
};

const std::array<std::string, 5> ShaderSourceFormat
{
    ".vert",
    ".frag",    
    ".geom",
    ".tess",
    ".comp"
  };



struct ShaderSourceAndPath
{
    std::vector<char> shaderSourceCode;
    std::string shaderSourceCodePath;
};

enum ShaderProgramPipelineType
{
    POINT_GRAPHICS,
    COMPUTE,
    RAYTRACING,
    
    COUT
};



struct ShaderGraphicPointEnum 
{
    
}


static_assert(static_cast<int>(ShaderProgramInfoFlagGraphic::COUNT) < 64, "ShaderProgrammInfoFlag is out of range os std::int64t");
static_assert(static_cast<int>(ShaderProgramInfoFlagCompute::COUNT) < 64, "ShaderProgrammInfoFlag is out of range os std::int64t");
static_assert(static_cast<int>(ShaderProgramInfoFlagCompute::COUNT) < 64, "ShaderProgrammInfoFlag is out of range os std::int64t");

struct ProgramShaderCreateInfo
{
    ShaderProgramPipelineType shaderProgramPipelineType;
    std::string prograShaderName;
    
    std::int64_t programShaderFlag;    
};


static inline LowLevelShaderStageType ShaderFormatToShaderType(const char* _formatWithPoint)
{
    for (int i = 0; i < static_cast<int>(LowLevelShaderStageType::COUNT); i++)
    {
        if (_stricmp(_formatWithPoint, ShaderSourceFormat[i].c_str()) == 0)
        {
            return static_cast<LowLevelShaderStageType>(i);
        }
    }
    
    // Return a default value or handle the error when no match is found
    return LowLevelShaderStageType::COUNT;
}
    


END_PCCORE