#pragma once

#include <string>

#include "core_header.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE
   

    class FileLoader
    {
    public:
        FileLoader() = delete;

        ~FileLoader() = delete;

        static uint8_t* LoadFile(char const* _filename, int* _x, int* _y, int* _comp, Channel _channel);

        static void FreeData(uint8_t* _file);
    };

END_PCCORE
