﻿#pragma once


#include <string>

#include "core_header.hpp"
#include <vector>
#include <array>
#include <variant>


BEGIN_PCCORE
#pragma region RHIFORMAT
    enum class RHIFormat
    {
        UNDIFINED,
        R8_UNORM,
        R8_SNORM,
        R8_USCALED,

        R32G32_SFLOAT,
        R32G32B32_SFLOAT,

        COUNT
    };

#pragma endregion


#pragma region Shader

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


    struct ShaderSourcePath
    {
        std::string shaderSourceCodePath;
        std::vector<char> spvCode;
    };

    enum class ShaderProgramPipelineType
    {
        POINT_GRAPHICS,
        COMPUTE,
        RAYTRACING,

        COUT
    };

    enum class VertexInputRate
    {
        VERTEX = 0,
        INSTANCE = 1,

        COUNT
    };

    struct VertexInputBindingDescrition
    {
        uint32_t binding = 0;
        uint32_t stride = 0;
        VertexInputRate vertexInputRate = VertexInputRate::VERTEX;
    };

    struct VertexAttributeDescription
    {
        uint32_t binding = 0;
        uint32_t location = 0;
        RHIFormat format = RHIFormat::UNDIFINED;
        uint32_t offset = 0;
    };


    struct ShaderGraphicPointInfo
    {
        std::vector<VertexInputBindingDescrition> vertexInputBindingDescritions;
        std::vector<VertexAttributeDescription> vertexAttributeDescriptions;
    };

    struct ShaderRayTracingInfo
    {
    };

    struct ShaderComputeInfo
    {
    };

    using ShaderInfoData = std::variant<ShaderGraphicPointInfo, ShaderRayTracingInfo, ShaderComputeInfo>;

    struct ShaderInfo
    {
        ShaderProgramPipelineType shaderProgramPipelineType;
        ShaderInfoData shaderInfoData;
    };

    struct ProgramShaderCreateInfo
    {
        std::string prograShaderName;
        ShaderInfo shaderInfo;
        // there shoulbe a renderpass handle
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

#pragma endregion Shader


END_PCCORE
