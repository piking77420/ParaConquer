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

    static constexpr std::array<std::string_view, static_cast<size_t>(ModelFormat::Count)> ModelSourceFormat
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

    static constexpr std::array<std::string_view,
        static_cast<size_t>(TextureFormat::Count)> TextureFormat
    {
        ".png",
        ".jpg",
        ".jpeg",
    };



    template <size_t Size>
    inline static bool IsFormatValid(const std::array<std::string_view, Size>& _format, const std::string& _fileFormat,
                                     uint32_t* _formatIndex)
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
    inline static bool GetFormatFromValue(const std::array<std::string_view, Size>& _format, T value,
                                          const char** _formatOut)
    {
        for (size_t i = 0; i < _format.size(); i++)
        {
            T v = static_cast<T>(i);

            if (value == v)
            {
                *_formatOut = _format[i].data();
                return true;
            }
        }
        return false;
    }

    inline std::string GetExtension(const std::string& filename)
    {
        size_t pos = filename.rfind('.');
        if (pos == std::string::npos) return {};
        return filename.substr(pos);
    }


END_PCCORE
