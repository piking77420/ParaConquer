#include "resources/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"
#include "rendering/vulkan/vulkan_buffer.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void Texture::Load(const fs::path& path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.generic_string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }
    
}
