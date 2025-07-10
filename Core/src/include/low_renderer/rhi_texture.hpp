#pragma once

#include "low_renderer/rhi_resource.hpp"

BEGIN_PCCORE

class RhiTexture : public PC_CORE::RhiResource
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiTexture)

    RhiTexture() = default;

    virtual ~RhiTexture() = default;
};

END_PCCORE