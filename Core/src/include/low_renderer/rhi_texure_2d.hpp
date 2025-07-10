#pragma once

#include "rhi_texture.hpp"

BEGIN_PCCORE

class RhiTexture2D : public RhiTexture
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiTexture2D)

    RhiTexture2D() = default;
    
    ~RhiTexture2D() override = default;
};

END_PCCORE
