#pragma once

#include <mutex>
#include <vector>

#include "EditorHeader.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_EDITOR_PCCORE
    class ShaderCompiler
    {
    public:

        std::vector<uint32_t> CompileFile(PC_CORE::GraphicAPI _api, const std::string& _filename);

        std::vector<uint32_t> CompileFile(PC_CORE::GraphicAPI _api, const std::wstring& _filename);

        ShaderCompiler();

        ~ShaderCompiler();

    private:
        std::mutex lock;
    };

END_EDITOR_PCCORE
