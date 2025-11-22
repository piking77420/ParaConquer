#pragma once

#include "RhiTexture.hpp"

BEGIN_PCCORE
    class RhiTexture2D : public RhiTexture
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(RhiTexture2D)

        RhiTexture2D(const CreateImageInfo& _createImageInfo) : RhiTexture(_createImageInfo)
        {
        }

        RhiTexture2D() = default;

        ~RhiTexture2D() override = default;
    };

END_PCCORE
