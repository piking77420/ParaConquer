#pragma once

#include "core_header.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE

class ShaderProgram;

    enum class MaterialType
{
    Opaque,
    Transparent,
    
};


class Material : public ResourceInterface<Material>
{
public:
    MaterialType m_MaterialType = MaterialType::Opaque;

    ResourceRef<Texture> m_albedo;

    PC_CORE_API Material();

    PC_CORE_API Material(const std::string& _name);

    PC_CORE_API ~Material();

    PC_CORE_API void Build();

    const ShaderProgramDescriptorSets* GetDescriptorSet();

private:
    ShaderProgramDescriptorSets* m_pShaderProgramDescriptorSets = nullptr;

    ShaderProgram* m_ShaderProgram;
};

REFLECT(Material, Resource)

END_PCCORE