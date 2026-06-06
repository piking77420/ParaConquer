#include <Io/FileLoader.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>
#include <utility>

namespace PC_CORE 
{
    bool PC_CORE::FileLoader::ReadFile(const std::filesystem::path& _path, std::vector<char>* _data)
    {
        if (_path.empty())
        {
            PC_LOGERROR("Path is empty");
            return false;
        }

        const std::filesystem::path parent = _path.parent_path();
        if (!std::filesystem::exists(parent))
        {
            PC_LOGERROR("ParentPath Path doesn't exist : {}", parent.generic_string());
        }

        const std::string sPath = _path.generic_string();
        std::ifstream file(sPath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            PC_LOGERROR("Failed to open file: {}", sPath);
            return false;
        }

        size_t fileSize = file.tellg();

        std::vector<char> buffer(fileSize);
        file.seekg(0);

        if (!file.read(buffer.data(), fileSize))
        {
            PC_LOGERROR("Failed to read file: {}", sPath);
            return false;
        }

        file.close();

        *_data = buffer;
        return true;
    }


    bool PC_CORE::FileLoader::WriteFile(const std::filesystem::path& _path, const void* _data, size_t _size,
        bool _createDirectories)
    {
        if (_path.empty())
        {
            PC_LOGERROR("Path is empty");
            return false;
        }
        const auto s = _path.generic_string();
        const std::filesystem::path parent = _path.parent_path();

        if (_createDirectories)
        {
            std::filesystem::create_directories(parent);
        }
        else
        {
            if (!std::filesystem::exists(parent))
            {
                PC_LOGERROR("Parent Path doesn't exist {}", s);
                return false;
            }
        }

        std::fstream f(s, std::ios::binary | std::ios::out | std::ios::trunc);

        if (!f.is_open())
        {
            PC_LOGERROR("File is not open {}", s);
            return false;
        }
        f.write(reinterpret_cast<const char*>(_data), _size);
        f.close();

        return true;
    }


    uint8_t* FileLoader::LoadImage(const char* _filename, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _channel, bool _isHdr)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Core);

        int channel = 0;
        if (_isHdr)
        {
            float* memory = stbi_loadf(_filename, _x, _y, &channel, static_cast<int>(_channel));
            *_comp = (_channel != RhiChannel::Default)
                ? _channel
                : static_cast<RhiChannel>(channel);
            return reinterpret_cast<uint8_t*>(memory);
        }
        
        uint8_t* memory = stbi_load(_filename, _x, _y, &channel, static_cast<int>(_channel));
        *_comp = (_channel != RhiChannel::Default)
            ? _channel
            : static_cast<RhiChannel>(channel);
        return memory;
    }

    uint8_t* FileLoader::LoadImageFromMemory(const uint8_t* _ptr, size_t _size, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _channel, bool _isHdr)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Core);

        int channel = 0;
        if (_isHdr)
        {
            float* memory = stbi_loadf_from_memory(_ptr, static_cast<int>(_size), _x, _y, &channel, static_cast<int>(_channel));
            *_comp = (_channel != RhiChannel::Default)
                ? _channel
                : static_cast<RhiChannel>(channel);
            return reinterpret_cast<uint8_t*>(memory);
        }

        uint8_t* memory = stbi_load_from_memory(_ptr, static_cast<int>(_size), _x, _y, &channel, static_cast<int>(_channel));
        *_comp = (_channel != RhiChannel::Default)
            ? _channel
            : static_cast<RhiChannel>(channel);
        return memory;
    }

    void FileLoader::FreeData(uint8_t* _file)
    {
        stbi_image_free(_file);
    }

    bool FileLoader::IsHdr(const char* _filename)
    {
        return stbi_is_hdr(_filename);
    }

} // namespace PC_CORE

