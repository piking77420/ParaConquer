#pragma once

#include <Resources/Resource.hpp>
#include <LowRenderer/RhiPipeline.hpp>

BEGIN_PCCORE
    class PC_CORE_API ShaderSourceBinary : public Resource
    {
    public:
        ShaderSourceBinary()
        {
            DYNAMIC_REFLECT_INIT;
        }

        ~ShaderSourceBinary() override = default;

        ShaderSourceBinary(const std::string& _name, const std::vector<uint32_t>* _sprivCode,
            PC_CORE::RhiPipeline::ShaderStageTypeBits _ShaderStageTypeBits, GraphicAPI _api);

        ShaderSourceBinary(std::string&& _name, const std::vector<uint32_t>* _sprivCode,
            PC_CORE::RhiPipeline::ShaderStageTypeBits _ShaderStageTypeBits, GraphicAPI _api);

        IMP_DYNAMIC_REFLECT()

        void OnParentReload(const Guid& _parentGuid) override;

        void WriteSprivToFile(const std::vector<uint32_t>* _sprivCode, GraphicAPI _api);

        std::vector<char> GetCode() const;

        PC_CORE::RhiPipeline::ShaderStageTypeBits GetShaderStageTypeBits() const
        {
            return m_ShaderStageTypeBits;
        }

    private:
        PC_CORE::RhiPipeline::ShaderStageTypeBits m_ShaderStageTypeBits;

        std::string m_Path;

        REFLECT(ShaderSourceBinary, PC_CORE::Resource);
    };

END_PCCORE
