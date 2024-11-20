#pragma once

#include "core_header.hpp"
#include "texture.hpp"
#include "rendering/render_harware_interface/descriptor_set.hpp"
#include "resources/shader_program.h"
#include "resources/material_instance.hpp"
#include <unordered_map>

#define MATERIAL_SET 1

BEGIN_PCCORE

// TO DO HANDLE DYNAMIC UNIFORM BUFFER


class Material : public ShaderProgram
{
public:
    PC_CORE_API Material(const ProgramShaderCreateInfo& _createInfo,
        const std::vector<ShaderSource*>& _shaderSources);

    PC_CORE_API Material() = default;

    PC_CORE_API Material(const fs::path& _path)
    {
        throw std::exception("Not Implemented Function");
    }

    PC_CORE_API ~Material() noexcept override = default;

    PC_CORE_API void CreateMaterialInstance(const std::string& _materialIntance);

    PC_CORE_API void DestroyMaterialInstace(const std::string& _materialIntance);

    PC_CORE_API void BuildMaterialInstance(const std::string& _materialIntance, std::array<DescriptorWriteSet, MAX_FRAMES_IN_FLIGHT>& _descritproWrite);
  
private:
    std::vector<std::array<DescriptorSetHandle, MAX_FRAMES_IN_FLIGHT>> m_MaterialInstanceDescriptor;
    std::unordered_map<std::string, uint32_t> m_MaterialInstanceMapToIndex;

    const PC_CORE::ShaderProgram* m_Shader = nullptr;
};

END_PCCORE