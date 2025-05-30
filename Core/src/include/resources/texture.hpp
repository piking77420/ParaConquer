#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/gpu_resource.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "low_renderer/rhi_typedef.h"


BEGIN_PCCORE


class Texture : public ResourceInterface<Texture>
{
public:
    PC_CORE_API Texture(const Texture& other) noexcept;

    PC_CORE_API Texture(Texture&& other) noexcept;

    PC_CORE_API Texture& operator=(const Texture& other) noexcept;

    PC_CORE_API Texture& operator=(Texture&& other) noexcept;

    PC_CORE_API void Build() override;
    
    PC_CORE_API Texture();

    PC_CORE_API Texture(const CreateImageInfo& createTextureInfo);
    
    PC_CORE_API Texture(const fs::path& _path);

    PC_CORE_API ~Texture() override;
    
    PC_CORE_API void Load(const std::array<std::string,6>& _maps);

    PC_CORE_API GPUHandleID GetGPUHandleID(int _frameIndex);

    PC_CORE_API RHIFormat GetRHIFormat() const;
    
private:
    int m_TextureChannel = -1;

    std::array<GPUHandleID, MAX_FRAMES_IN_FLIGHT> m_TextureHandles;

    RHIFormat m_Format;

    PC_CORE_API void CreateFromCreateInfo(const CreateImageInfo& createTextureInfo);

    PC_CORE_API void LoadFromFile(const fs::path& _path);
};

REFLECT(Texture, Resource)


END_PCCORE