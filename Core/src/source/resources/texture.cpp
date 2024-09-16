#include "resources/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"


using namespace PC_CORE;

Texture::~Texture()
{
    //vulkanTexture.Destroy();
}
void Texture::SetPath(const fs::path& path)
{
    stbi_uc* pixels = stbi_load(path.generic_string().c_str(), &textureSize.x, &textureSize.y, &textureChannel, STBI_rgb_alpha);
    //VkDeviceSize dataImageSize=  {};
   // dataImageSize = textureSize.x * textureSize.y * 4;

    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }
    
    //vulkanTexture.Init(pixels, dataImageSize, textureSize);
    stbi_image_free(pixels);


    name = path.filename().generic_string();
    format = path.extension().generic_string();
}

void Texture::Load(std::array<std::string, 6>& _maps)
{
    std::array<void*, 6> datas = {};
    for (size_t i = 0; i < datas.size(); i++)
    {
        datas[i] = stbi_load(_maps[i].c_str(), &textureSize.x, &textureSize.y, &textureChannel, STBI_rgb_alpha);
    }
    //vulkanTexture.Init(datas, textureSize.x , textureSize.y);
    for (size_t i = 0; i < datas.size(); i++)
    {
        stbi_image_free(datas[i]);

    }

    const fs::path path(_maps[0]);
    
    name = path.filename().generic_string();
    format = path.extension().generic_string();
}
