#pragma once

#include "CoreHeader.hpp"
#include "Texture.hpp"
#include "LowRenderer/RhiTexture3d.hpp"

BEGIN_PCCORE
class PC_CORE_API Texture3D : public Texture
{
public:
    std::shared_ptr<RhiResource> GetRhiHandle() const
    {
        return m_RhiTexture3D;
    }

    IMP_DYNAMIC_REFLECT()

    DEFAULT_COPY_MOVE_OPERATIONS(Texture3D)

    Texture3D() : Texture()
    {
        DYNAMIC_REFLECT_INIT
    }

    Texture3D(const std::string& _name, const std::array<std::string, 6>& _maps);
    
    ~Texture3D() override = default;

protected:
    std::shared_ptr<RhiTexture3D> m_RhiTexture3D;

    std::vector<std::filesystem::path> m_FileSources;

    REFLECT(Texture3D, Texture)
    REFLECT_MEMBER(Texture3D, m_FileSources)

    
};

END_PCCORE