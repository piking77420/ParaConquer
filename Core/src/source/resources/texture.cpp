#include "resources/texture.hpp"

#include "log.hpp"
#include "rendering/render_harware_interface/RHI.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/file_loader.hpp"


using namespace PC_CORE;

Texture::~Texture()
{
    //vulkanTexture.Destroy();
}
void Texture::SetPath(const fs::path& path)
{
    uint8_t* pixels = FileLoader::LoadFile(path.generic_string().c_str(), &textureSize.x, &textureSize.y, &textureChannel, Channel::RGBA);
    //VkDeviceSize dataImageSize=  {};
   // dataImageSize = textureSize.x * textureSize.y * 4;
    const size_t size = textureSize.x * textureSize.y * 4;

    GPUBufferHandle buffer;
    RHI::BufferData(size, pixels);
    
    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }
    
    //vulkanTexture.Init(pixels, dataImageSize, textureSize);
    FileLoader::FreeData(pixels);


    name = path.filename().generic_string();
    format = path.extension().generic_string();
}

void Texture::Load(std::array<std::string, 6>& _maps)
{
    std::array<void*, 6> datas = {};
    for (size_t i = 0; i < datas.size(); i++)
    {
        datas[i] =  FileLoader::LoadFile(_maps[i].c_str(), &textureSize.x, &textureSize.y, &textureChannel, Channel::RGBA);
    }
    //vulkanTexture.Init(datas, textureSize.x , textureSize.y);
    for (size_t i = 0; i < datas.size(); i++)
    {
        FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
    }

    const fs::path path(_maps[0]);
    
    name = path.filename().generic_string();
    format = path.extension().generic_string();
}
