#pragma once

#include <string>

#include "core_header.hpp"

BEGIN_PCCORE
    enum class Channel
    {
        DEFAULT = 0, // only used for desired_channels

        GREY = 1,
        ALPHA = 2,
        RGB = 3,
        RGBA = 4
    };

    class FileLoader
    {
    public:
        FileLoader() = delete;

        ~FileLoader() = delete;

        static uint8_t* LoadFile(char const* _filename, int* _x, int* _y, int* _comp, Channel _channel);

        static void FreeData(uint8_t* _file);
    };

END_PCCORE
