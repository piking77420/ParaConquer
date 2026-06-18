#pragma once

#include <Resources/Resource.hpp>
#include <LowRenderer/RhiPipeline.hpp>

BEGIN_PCCORE
    class PC_CORE_API ShaderSourceBinary : public Resource
    {
    public:
        PC_CORE::RhiPipeline::ShaderStageTypeBits GetShaderStageType() const
        {
            return m_ShaderStageType;
        }

        IMP_DYNAMIC_REFLECT()

        ShaderSourceBinary()
        {
            DYNAMIC_REFLECT_INIT;
        }

        void OnParentReload(const Guid& _parentGuid) override;

        void WriteSprivToFile(const std::vector<uint32_t>* _sprivCode, GraphicAPI _api);

        std::vector<char> GetCode() const;

        ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode,
            RhiPipeline::ShaderStageTypeBits _shaderStageType, GraphicAPI _api);

        ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode,
            RhiPipeline::ShaderStageTypeBits _shaderStageType, GraphicAPI _api);

        ~ShaderSourceBinary() override = default;

    private:
        RhiPipeline::ShaderStageTypeBits m_ShaderStageType;

        std::string m_Path;

        REFLECT(ShaderSourceBinary, PC_CORE::Resource);
    };

END_PCCORE
