#pragma once

#include "core_header.hpp"
#include "texture.hpp"
#include "low_renderer/rhi_texture_3d.hpp"

BEGIN_PCCORE
class PC_CORE_API Texture3d : public Texture
{
public:
   

    std::shared_ptr<RhiResource> GetRhiHandle() const
    {
        return m_RhiTexture3D;
    }
    
    TextureType GetType() const
    {
        return m_Texture3DType;
    }

    IMP_DYNAMIC_REFLECT()

    DEFAULT_COPY_MOVE_OPERATIONS(Texture3d)

    Texture3d() : Texture()
    {
        DYNAMIC_REFLECT_INIT
    }

    ~Texture3d() override = default;

protected:
    std::shared_ptr<RhiTexture3D> m_RhiTexture3D;

    TextureType m_Texture3DType = TextureType::Count;
};

END_PCCORE