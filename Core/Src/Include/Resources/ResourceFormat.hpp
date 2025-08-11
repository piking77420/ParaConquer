#pragma once


#include <array>
#include <string_view>
using namespace std::literals;

#include "CoreHeader.hpp"

BEGIN_PCCORE

// TO DO TO STRING VIEW

enum struct ModelFormat : uint8_t
{
    Obj,
    Gltf,

    Count
};

static inline  std::array<std::string, static_cast<size_t>(ModelFormat::Count)> ModelSourceFormat
{
    ".obj",
    ".gltf",
};

enum struct TextureFormat : uint8_t
{
    Png,
    Jpg,
    Jpeg,

    Count
};

static inline  std::array<std::string, static_cast<size_t>(TextureFormat::Count)> TextureFormat
{
    ".png",
    ".jpg",
    ".jpeg",
};



enum struct ShaderStageType : uint8_t
{
    Vertex,
    TessControl,
    Tessevaluation,
    Geometry,
    Fragment,
    Compute,
    Raygen,
    Intersect,
    Anyhit,
    Closesthit,
    Miss,
    Callable,
    Task,
    Mesh,

    Count
};


const std::array<std::string, static_cast<size_t>(ShaderStageType::Count)> ShaderSourceFormat =
{
    ".vert",
    ".tessc",
    ".tessv",
    ".geom",
    ".frag",
    ".comp",
    ".raygen",
    ".intersect",
    ".anyhit",
    ".closesthit",
    ".miss",
    ".callable",
    ".task"
    ".mesh",
};

template <size_t Size>
inline  static bool IsFormatValid(const std::array<std::string, Size>& _format, const std::string& _fileFormat, uint32_t* _formatIndex)
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



template <typename T, size_t Size>
inline  static bool GetFormatFromValue(const std::array<std::string, Size>& _format, T value, const char** _formatOut)
{

    for (size_t i = 0; i < _format.size(); i++)
    {
        T v = static_cast<T>(i);

        if (value == v)
        {
            *_formatOut = _format[i].c_str();
            return true;
        }
    }
    return false;
}

inline std::string GetExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return {};
    return filename.substr(pos);
}




END_PCCORE