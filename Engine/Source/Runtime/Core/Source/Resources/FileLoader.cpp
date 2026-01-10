#include "Resources/FileLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

uint8_t* PC_CORE::FileLoader::LoadImage(const char* _filename, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _req_comp)
{
    int channel = 0;
    uint8_t* memory = stbi_load(_filename, _x, _y, &channel, static_cast<int>(_req_comp));
    *_comp = (_req_comp != RhiChannel::Default)
        ? _req_comp
        : static_cast<RhiChannel>(channel);
    return memory;
}

uint8_t* PC_CORE::FileLoader::LoadImageFromMemory(const uint8_t* _ptr, size_t _size, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _req_comp)
{
    int channel = 0;
    uint8_t* memory = stbi_load_from_memory(_ptr, static_cast<int>(_size), _x, _y, &channel, static_cast<int>(_req_comp));
    *_comp = (_req_comp != RhiChannel::Default)
        ? _req_comp
        : static_cast<RhiChannel>(channel);
    return memory;
}

void PC_CORE::FileLoader::FreeData(uint8_t* _file)
{
    stbi_image_free(_file);
}

bool PC_CORE::FileLoader::IsHdr(const char* _filename)
{
    return stbi_is_hdr(_filename);
}

PC_CORE::Image::Image(const std::string& _path, PC_CORE::RhiChannel _desireChannel)
    : m_IsHDR(FileLoader::IsHdr(_path.c_str()))
{
    int x = -1;
    int y = -1;
    m_Data.reset(FileLoader::LoadImage(_path.c_str(), &x, &y, &m_Channel, _desireChannel));

    if (x != -1 && y != -1)
    {
        m_Width = static_cast<uint32_t>(x);
        m_Height = static_cast<uint32_t>(y);
        ComputeDataSize();
    }
    else
    {
        PC_LOGERROR("Something went wrong {}", _path);
    }

}

PC_CORE::Image::Image(const char* _path , PC_CORE::RhiChannel _desireChannel)
    : m_IsHDR(FileLoader::IsHdr(_path))
{
    int x = -1;
    int y = -1;
    m_Data.reset(FileLoader::LoadImage(_path, &x, &y, &m_Channel, _desireChannel));

    if (x != -1 && y != -1)
    {
        m_Width = static_cast<uint32_t>(x);
        m_Height = static_cast<uint32_t>(y);
        ComputeDataSize();
    }
    else
    {
        PC_LOGERROR("Something went wrong {}", _path);
    }
}

PC_CORE::Image::Image(const uint8_t* _ptr, size_t _size, const char* _name, PC_CORE::RhiChannel _Channel)
{
    int x = -1;
    int y = -1;
    m_Data.reset(FileLoader::LoadImageFromMemory(_ptr, _size, &x, &y, &m_Channel, _Channel));

    if (x != -1 && y != -1)
    {
        m_Width = static_cast<uint32_t>(x);
        m_Height = static_cast<uint32_t>(y);
        ComputeDataSize();
    }
    else
    {
        PC_LOGERROR("Something went wrong {}", _name);
    }
}

void PC_CORE::Image::ComputeDataSize()
{
    m_SizeInBytes = static_cast<size_t>(m_Width) *
        static_cast<size_t>(m_Height) *
        static_cast<size_t>(m_Channel) * (m_IsHDR ? sizeof(float) : sizeof(uint8_t));

};
