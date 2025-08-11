#pragma once

#include <String>

#include "CoreHeader.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_PCCORE
   

    class FileLoader
    {
    public:
        FileLoader() = delete;

        ~FileLoader() = delete;

        static uint8_t* LoadFile(char const* _filename, int* _x, int* _y, Channel* _comp, Channel _channel);

        static void FreeData(uint8_t* _file);
    };

END_PCCORE
