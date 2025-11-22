#pragma once

#include <Vector>
#include <String>

#include "RhiTexture.hpp"

BEGIN_PCCORE
    class PC_CORE_API RhiTexture3D : public RhiTexture
    {
    public:
        RhiTexture3D(const CreateImageInfo& _createImageInfo) : RhiTexture(_createImageInfo)
        {
        }

        RhiTexture3D() = default;

        ~RhiTexture3D() override = default;
    };

END_PCCORE
