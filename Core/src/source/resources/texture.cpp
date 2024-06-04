#include "resources/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"
#include "rendering/vulkan/vulkan_buffer.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

Texture::~Texture()
{
    vulkanTexture.Destroy();
}

void Texture::Load(const fs::path& path)
{
    stbi_uc* pixels = stbi_load(path.generic_string().c_str(), &textureSize.x, &textureSize.y, &textureChannel, STBI_rgb_alpha);
    VkDeviceSize dataImageSize=  {};
    dataImageSize = textureSize.x * textureSize.y * 4;

    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }


    vulkanTexture.Init(pixels, dataImageSize, textureSize);

}
