#pragma once

#include "core_header.hpp"
#include <filesystem>
#include <array>

#include "guid.hpp"

namespace fs = std::filesystem;

BEGIN_PCCORE

class IResource
{
public:
    std::string name;
    
    std::string format;

    fs::path resourcePath;

    Guid guid;
    
    IResource();
    
    virtual ~IResource() = default;

    virtual void Load(const fs::path& path) {};

    template <size_t _Size>
    static bool IsFormatValid(std::array<std::string,_Size> _format, const std::string& _fileFormat, uint32_t* _formatIndex);

};

template <size_t _Size>
bool IResource::IsFormatValid(std::array<std::string, _Size> _format, const std::string& _fileFormat, uint32_t* _formatIndex)
{
    for (size_t i = 0; i < _format.size(); i++)
    {
        if (_format[i] == _fileFormat)
        {
            *_formatIndex = static_cast<uint32_t>(i);
            return true;
        }
    }

    _formatIndex = nullptr;
    return false;
}

END_PCCORE
