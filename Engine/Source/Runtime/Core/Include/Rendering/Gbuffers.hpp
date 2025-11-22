#pragma once

#include <Memory>

#include "RenderingTypedef.h"
#include "LowRenderer/FrameBuffer.hpp"
#include "Resources/Texture2d.hpp"

BEGIN_PCCORE
    class Gbuffers
    {
    public:
        std::array<Texture2D, static_cast<uint8_t>(GbufferType::Count)> gbuffers;

        PC_CORE_API Texture2D& GetTexture(GbufferType type);

        PC_CORE_API const Texture2D& GetTexture(GbufferType type) const;

        PC_CORE_API void CreateGBuffers(Tbx::Vector2i _size);

        // Create also Textures
        PC_CORE_API Gbuffers(Tbx::Vector2i _size);

        PC_CORE_API Gbuffers() = default;

        PC_CORE_API ~Gbuffers() = default;
    };

END_PCCORE
