#include "low_renderer/rhi_texture.hpp"


PC_CORE::RhiTexture::RhiTexture(const PC_CORE::CreateImageInfo& _createImageInfo) : m_MipLevelCount(_createImageInfo.mipsLevels),
m_TextureUsage(_createImageInfo.textureUsage), m_RhiFormat(_createImageInfo.format)
{
	assert(_createImageInfo.textureType != PC_CORE::TextureType::TextureArray2D && "Unsure correct Layers");




}
