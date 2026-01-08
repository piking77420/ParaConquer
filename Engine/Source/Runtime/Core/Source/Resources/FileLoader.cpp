#include "Resources/FileLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

uint8_t* PC_CORE::FileLoader::LoadImage(const char* _filename, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _req_comp)
{
    int channel = 0;
    uint8_t* memory = stbi_load(_filename, _x, _y, &channel, static_cast<int>(_req_comp));
    *_comp = static_cast<PC_CORE::RhiChannel>(channel);
    return memory;
}

uint8_t* PC_CORE::FileLoader::LoadImageFromMemory(const uint8_t* _ptr, size_t _size, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _req_comp)
{
    int channel = 0;
    uint8_t* memory = stbi_load_from_memory(_ptr, static_cast<int>(_size), _x, _y, &channel, static_cast<int>(_req_comp));
    *_comp = static_cast<PC_CORE::RhiChannel>(channel);
    return memory;
}

void PC_CORE::FileLoader::FreeData(uint8_t* _file)
{
    stbi_image_free(_file);
}

PC_CORE::Image::Image(const std::string& _path, PC_CORE::RhiChannel _desireChannel)
{
    int x = -1;
    int y = -1;
    m_Data.reset(FileLoader::LoadImage(_path.c_str(), &x, &y, &m_Channel, _desireChannel));

    if (x != -1 && y != -1)
    {
        m_Widht = static_cast<uint32_t>(x);
        m_Height = static_cast<uint32_t>(y);

    }
    else
    {
        PC_LOGERROR("Something went wrong {}", _path);
    }

}

PC_CORE::Image::Image(const char* _path , PC_CORE::RhiChannel _desireChannel)
{
    int x = -1;
    int y = -1;
    m_Data.reset(FileLoader::LoadImage(_path, &x, &y, &m_Channel, _desireChannel));

    if (x != -1 && y != -1)
    {
        m_Widht = static_cast<uint32_t>(x);
        m_Height = static_cast<uint32_t>(y);

    }
    else
    {
        PC_LOGERROR("Something went wrong {}", _path);
    }
}

PC_CORE::Image::Image(const uint8_t* _ptr, size_t _size, const char* _name)
{
    int x = -1;
    int y = -1;
    m_Data.reset(FileLoader::LoadImageFromMemory(_ptr, _size, &x, &y, &m_Channel, RhiChannel::Default));

    if (x != -1 && y != -1)
    {
        m_Widht = static_cast<uint32_t>(x);
        m_Height = static_cast<uint32_t>(y);
    }
    else
    {
        PC_LOGERROR("Something went wrong {}", _name);
    }
}
