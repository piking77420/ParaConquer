#include "Resources/FileLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

uint8_t* PC_CORE::FileLoader::LoadFile(char const* _filename, int* _x, int* _y, Channel* _comp, Channel _req_comp)
{
    int channel = 0;
    uint8_t* memory = stbi_load(_filename, _x, _y, &channel, static_cast<int>(_req_comp));
    *_comp = static_cast<Channel>(channel);
    return memory;
}

void PC_CORE::FileLoader::FreeData(uint8_t* _file)
{
    stbi_image_free(_file);
}
