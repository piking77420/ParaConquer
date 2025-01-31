#include "resources/file_loader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

uint8_t* PC_CORE::FileLoader::LoadFile(char const* _filename, int* _x, int* _y, int* _comp, Channel _req_comp)
{
    

    return stbi_load(_filename, _x, _y, _comp, static_cast<int>(_req_comp));
}

void PC_CORE::FileLoader::FreeData(uint8_t* _file)
{
    stbi_image_free(_file);
}
