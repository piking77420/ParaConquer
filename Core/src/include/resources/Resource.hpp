#pragma once

#include "core_header.hpp"
#include <filesystem>
#include <array>
#include "log.hpp"

#include "guid.hpp"

namespace fs = std::filesystem;

BEGIN_PCCORE

class Resource
{
public:
    std::string name;
    
    std::string format;

    fs::path path;

    Guid guid;

    PC_CORE_API Resource& operator=(Resource&& _other) = default;
    
    PC_CORE_API Resource();

    PC_CORE_API Resource(const fs::path& _path, const Guid& _guid) {}

    PC_CORE_API Resource(const fs::path& _path);

    PC_CORE_API virtual ~Resource() = default;

    PC_CORE_API void SetPath(const fs::path& _path);

    PC_CORE_API virtual std::vector<char> GetData() { return {}; };
    
    PC_CORE_API virtual void WriteFile(const fs::path& path) {};

    template <size_t _Size>
    static bool IsFormatValid(const std::array<std::string,_Size>& _format, const std::string& _fileFormat, uint32_t* _formatIndex);

};

template <size_t _Size>
bool Resource::IsFormatValid(const std::array<std::string, _Size>& _format, const std::string& _fileFormat, uint32_t* _formatIndex)
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
