#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/rhi_texure_2d.hpp"
#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "low_renderer/rhi_typedef.h"
#include "rendering/buffer/gpu_buffer.hpp"


BEGIN_PCCORE
    class Texture : public Resource , public IGpuResource
{
public:
    
    std::shared_ptr<RhiResource> GetRhiHandle() const override
    {
        return m_Texture2D;
    }
    
    std::shared_ptr<RhiTexture2D> GetRhiTexture2D()
    {
        return m_Texture2D;
    }
    
    PC_CORE_API IMP_DYNAMIC_REFLECT()
    
    DEFAULT_COPY_MOVE_OPERATIONS(Texture)
    
    PC_CORE_API void Build() override;
    
    PC_CORE_API RHIFormat GetRHIFormat() const;
    
    PC_CORE_API Texture();

    PC_CORE_API Texture(const CreateImageInfo& createTextureInfo);
    
    PC_CORE_API Texture(const fs::path& _path);

    PC_CORE_API ~Texture() override;
    
private:
    int m_TextureChannel = -1;

    std::shared_ptr<RhiTexture2D> m_Texture2D;
    
    RHIFormat m_Format;
    
    PC_CORE_API void LoadFromFile(const fs::path& _path);
};

REFLECT(Texture, Resource)


END_PCCORE