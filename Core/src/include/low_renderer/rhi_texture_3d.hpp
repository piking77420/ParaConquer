#pragma once

#include <vector>
#include <string>

#include "rhi_texture.hpp"

BEGIN_PCCORE



class PC_CORE_API RhiTexture3D : public RhiTexture
{
public:
    
    RhiTexture3D() = default;

    virtual ~RhiTexture3D() = default;
};
END_PCCORE