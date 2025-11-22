#pragma once

#include <vector>
#include <filesystem>

#include "CoreHeader.hpp"


BEGIN_PCCORE
    class InOut
    {
    public:
        PC_CORE_API static bool ReadFile(const std::filesystem::path& _path, std::vector<char>* _data);

        PC_CORE_API static bool WriteFile(const std::filesystem::path& _path, const void* _data, size_t _size,
                                          bool _createDirectories = false);
    };

END_PCCORE
