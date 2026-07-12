#pragma once

#include <array>

#include "RhiResource.hpp"

BEGIN_PCCORE

class RhiRenderPass;
class ShaderSourceBinary;

class RhiPipeline : public RhiObjectT<RhiPipeline>
{
public:
    enum ShaderStageTypeBits : uint16_t
    {
        Vertex = 1 << 0,
        Hull = 1 << 1,
        Domain = 1 << 2,
        Geometry = 1 << 3,
        Pixel = 1 << 4,
        Compute = 1 << 5,
        Raygen = 1 << 6,
        Intersection = 1 << 7,
        Anyhit = 1 << 8,
        Closesthit = 1 << 9,
        Miss = 1 << 10,
        Callable = 1 << 11,
        Amp = 1 << 12,
        Mesh = 1 << 13,

        ShaderStageTypeCount = 14
    };

     
    static constexpr const std::array<std::string_view, static_cast<size_t>(ShaderStageTypeBits::ShaderStageTypeCount)> ShaderSourceFormat =
    {
        ".vs.hlsl",
        ".hs.hlsl",
        ".ds.hlsl",
        ".gs.hlsl",
        ".ps.hlsl",
        ".cs.hlsl",
        ".rgen.hlsl",
        ".rint.hlsl",
        ".ahit.hlsl",
        ".chit.hlsl",
        ".miss.hlsl",
        ".call.hlsl",
        ".as.hlsl",
        ".ms.hlsl",
    };

    struct ShaderModuleBinary
    {
        std::vector<char> Code;
        RhiPipeline::ShaderStageTypeBits ShaderStage;
    };

    static constexpr bool FormatToShaderStageType(std::underlying_type_t<ShaderStageTypeBits>* _ShaderStageTypeBits, std::string_view _SvFormat)
    {
        auto it = std::ranges::find_if(ShaderSourceFormat, [&](const std::string_view& _Other) {return _Other == _SvFormat; });
        if (it == ShaderSourceFormat.end())
            return false;


        const uint32_t Index = static_cast<uint32_t>(std::distance(ShaderSourceFormat.begin(), it));
        *_ShaderStageTypeBits |= static_cast<std::underlying_type_t<ShaderStageTypeBits>>(1 << Index);
        return true;
    }

    static constexpr bool FormatToShaderStageTypeBits(ShaderStageTypeBits* _ShaderStageTypeBits, std::string_view _SvFormat)
    {
        auto it = std::ranges::find_if(ShaderSourceFormat, [&](const std::string_view& _Other) {return _Other == _SvFormat; });
        if (it == ShaderSourceFormat.end())
            return false;

        const uint32_t Index = static_cast<uint32_t>(std::distance(ShaderSourceFormat.begin(), it));
        *_ShaderStageTypeBits = static_cast<ShaderStageTypeBits>(1 << Index);
        return true;
    }

public:
    PC_CORE_API RhiPipeline(Rhi& _Rhi);

    PC_CORE_API ~RhiPipeline() override = default;
 
    PC_CORE_API RhiPipeline& SetShaderModules(const std::vector<ShaderModuleBinary>& _ShaderModules);

protected:
    std::optional<const std::vector<ShaderModuleBinary>> m_Modules;
};

REFLECT(RhiPipeline, RhiResource);

END_PCCORE

using RhiShader = PC_CORE::RhiPipeline;
using RhiShaderStageBits = PC_CORE::RhiPipeline::ShaderStageTypeBits;
using RhiShaderStageTypeFlag = uint16_t;
