#pragma once

#include <Memory>

#include "RenderingTypedef.h"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "Resources/Texture2d.hpp"

BEGIN_PCCORE
enum struct GbufferType : std::uint8_t
{
Albedo,
Normal,
RoughnessMetallicAo,
WorldPosition,

Count
};

inline std::string GbufferTypeToString(GbufferType _gbufferType)
{
switch (_gbufferType)
{
case GbufferType::Albedo:
    return "Albedo";
case GbufferType::Normal:
    return "Normal";
case GbufferType::RoughnessMetallicAo:
    return "RoughnessMetallicAo";
case GbufferType::WorldPosition:
    return "WorldPosition";
case GbufferType::Count:
    return "";
}
return "";
}

class Gbuffers
{
public:
    std::array<std::unique_ptr<RhiTexture>, static_cast<uint8_t>(GbufferType::Count)> gbuffers;

    PC_CORE_API RhiTexture& GetTexture(GbufferType type);

    PC_CORE_API const RhiTexture& GetTexture(GbufferType type) const;

    PC_CORE_API void CreateGBuffers(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _size);

    // Create also Textures
    PC_CORE_API Gbuffers(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _size);

    PC_CORE_API Gbuffers() = default;

    PC_CORE_API ~Gbuffers() = default;
};

END_PCCORE
