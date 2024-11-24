#pragma once

#include "core_header.hpp"
#include "texture.hpp"
#include "rendering/render_harware_interface/descriptor_set.hpp"
#include "resources/shader_program.h"
#include "resources/material_instance.hpp"
#include <unordered_map>

#include "rendering/render_harware_interface/command_buffer.h"

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

    PC_CORE_API void CreateMaterialInstance(MaterialInstance* _materialInstance);

    PC_CORE_API void DestroyMaterialInstace(MaterialInstance* _materialInstance);

    PC_CORE_API void UpdateMaterialInstance(const MaterialInstance& _materialInstance, DescriptorWriteSet* _descriptorWriteSets, uint32_t descriptorCount, uint32_t imageIndex) const;

    PC_CORE_API DescriptorSetHandle GetDesciptorSet(const MaterialInstance& _materialInstance) const;


private:
    mutable std::vector<std::array<DescriptorSetHandle, MAX_FRAMES_IN_FLIGHT>> m_MaterialInstanceDescriptor;
    
    std::unordered_map<std::string, uint32_t> m_MaterialInstanceMapToIndex;
};

END_PCCORE