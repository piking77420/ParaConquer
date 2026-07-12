#pragma once

#include <expected>

#include "EditorHeader.hpp"
#include "Resources/Resource.hpp"
#include "LowRenderer/RhiTypedef.h"
#include <Rendering/RenderingTypedef.h>
#include "Rendering/ShaderProgram.hpp"


BEGIN_EDITOR_PCCORE

    class ShaderSource : public PC_CORE::Resource
    {
    public:
        enum class GenerateVariantResult
        {
            Failed,
            Compiling,
            Sucess,
        };

        explicit ShaderSource();

        explicit ShaderSource(const std::string& _name, const std::filesystem::path& _path);

        ~ShaderSource() override = default;

        IMP_DYNAMIC_REFLECT()

        void Reload() override;

        GenerateVariantResult GenerateVariant(const std::string& _VariantPath, PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlag = 0);

    private:
        PC_CORE::RhiPipeline::ShaderStageTypeBits m_ShaderType;

        std::filesystem::path m_PathToSource;

        static std::vector<std::wstring> GetDefineFromShaderFeatures(PC_CORE::Rendering::ShaderFeatureFlags _Flags);

    };

    REFLECT(ShaderSource)

END_PCCORE
