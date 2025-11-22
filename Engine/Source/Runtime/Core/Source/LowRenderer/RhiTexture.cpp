#include "LowRenderer/RhiTexture.hpp"


PC_CORE::RhiTexture::RhiTexture(const CreateImageInfo& _createImageInfo) : m_MipLevelCount(_createImageInfo.MipsLevels),
                                                                           m_TextureUsage(
                                                                               _createImageInfo.TextureUsage),
                                                                           m_RhiFormat(_createImageInfo.Format)
{
    assert(_createImageInfo.TextureType != PC_CORE::TextureType::TextureArray2D && "Unsure correct Layers");
}
