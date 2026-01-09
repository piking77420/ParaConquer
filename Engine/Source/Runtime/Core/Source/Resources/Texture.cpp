#include "resources/texture.hpp"

#include "LowRenderer/RhiTexture.hpp"

namespace PC_CORE
{

    Texture::Texture(std::unique_ptr<PC_CORE::RhiTexture> _Texture)
        : Resource(std::string(_Texture->GetName().data()))
        , m_RhiTexture(std::move(_Texture))
    {

    }

}
