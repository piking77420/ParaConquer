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


class Material : public Resource
{
public:
    MaterialType m_MaterialType = MaterialType::Opaque;

    ResourceRef<Texture> m_albedo;

    PC_CORE_API IMP_DYNAMIC_REFLECT();

    PC_CORE_API Material();

    PC_CORE_API Material(const std::string& _name);

    PC_CORE_API ~Material();

    PC_CORE_API void Build() override;

    const ShaderProgramDescriptorSets* GetDescriptorSet() const
    {
        return m_pShaderProgramDescriptorSets;
    }

private:
    ShaderProgramDescriptorSets* m_pShaderProgramDescriptorSets = nullptr;

    ResourceRef<ShaderProgram> m_ShaderProgram;
};

REFLECT(Material, Resource)

END_PCCORE