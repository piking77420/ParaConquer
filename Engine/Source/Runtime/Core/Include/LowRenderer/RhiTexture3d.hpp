#pragma once

#include <Vector>
#include <String>

#include "RhiTexture.hpp"

BEGIN_PCCORE



class PC_CORE_API RhiTexture3D : public RhiTexture
{
public:

    RhiTexture3D(const PC_CORE::CreateImageInfo& _createImageInfo) : RhiTexture(_createImageInfo) {}

    RhiTexture3D() = default;

    virtual ~RhiTexture3D() = default;
};
END_PCCORE