#pragma once

#include "core_header.hpp"
#include "Resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE
    class Texture : public Resource
{
public:
    Tbx::Vector2i textureSize;
    
    int textureChannel = -1;
    
    ~Texture() override;
    
    PC_CORE_API void SetPath(const fs::path& path) override;

    PC_CORE_API void Load(std::array<std::string,6>& _maps);

    PC_CORE::ImageHandle GetImageHandle();

    PC_CORE::ImageHandle GetImageViewHandle();

    PC_CORE::ImageHandle GetSamplerHandle();


private:
    PC_CORE::ImageHandle m_ImageHandle;

    PC_CORE::ImageViewHandle m_ImageViewHandle;

    PC_CORE::SamplerHandle m_SamplerHandle;
};


END_PCCORE