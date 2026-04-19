#pragma once

#include "EditorHeader.hpp"
#include "Resources/Resource.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Rendering/ShaderProgram.hpp"


BEGIN_EDITOR_PCCORE

    enum ShaderFeatureFlagBits : size_t {
        None = 0,
        Lit = 1 << 0,
        UseUV = 1 << 1,
        UseNormalMap = 1 << 2,
        UseColor = 1 << 3,
        DrawTriangle = 1 << 4,
    };

    using ShaderFeatureFlag = size_t;

    class ShaderSource : public PC_CORE::Resource
    {
    public:
        explicit ShaderSource();

        explicit ShaderSource(const std::string& _name, ShaderFeatureFlag _ShaderFeatureFlag = 0);

        explicit ShaderSource(const std::string& _name, const std::filesystem::path& path, ShaderFeatureFlag _ShaderFeatureFlag = 0);

        ~ShaderSource() override = default;

        IMP_DYNAMIC_REFLECT()

        void Reload() override;

        bool GetCompiledShaderSource(std::vector<uint32_t>* _buffer);

        ShaderFeatureFlag GetShaderFeatures() const
        {
            return m_ShaderFeatureFlag;
        }

    private:
        PC_CORE::RhiShaderProgram::ShaderStageTypeBits m_ShaderType;

        std::filesystem::path m_PathToSource;

        ShaderFeatureFlag m_ShaderFeatureFlag = 0;

        std::vector<std::wstring> GetDefineFromShaderFeatures() const;

        std::string GetShaderBinaryPath();

        static void AddPreProcessorDefVulkan();
    };

    REFLECT(ShaderSource)

END_PCCORE
