#pragma once

#include "core_header.hpp"
#include "material_instance.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "resources/texture_2d.hpp"

BEGIN_PCCORE

class ShaderProgram;

    enum class MaterialType
{
    Opaque,
    Transparent,
    
};

enum struct MaterialAttribute : uint8_t
{
    Color,
    Roughness,
    Metallic,
    Normal,
    Ao,
};

enum struct MaterialValueType : uint8_t
{
    Scalar,
    Vec2,
    Vec3,
    Vec4,
    TextureSample,
};



class Material : public Resource
{
public:
    MaterialType materialType = MaterialType::Opaque;

    ResourceRef<Texture2D> albedo;

    ResourceRef<Texture2D> metallic;

    ResourceRef<Texture2D> roughess;

    ResourceRef<Texture2D> normal;


    std::shared_ptr<MaterialInstance> CreateMaterialInstance();

    PC_CORE_API IMP_DYNAMIC_REFLECT();

    PC_CORE_API Material();

    PC_CORE_API Material(const std::string& _name);

    PC_CORE_API ~Material();

    PC_CORE_API void Build();

    const ShaderProgramDescriptorSets* GetDescriptorSet() const
    {
        return m_PShaderProgramDescriptorSets;
    }

private:
    ShaderProgramDescriptorSets* m_PShaderProgramDescriptorSets = nullptr;

    ResourceRef<ShaderProgram> m_ShaderProgram;

    std::vector<std::shared_ptr<MaterialInstance>> m_MaterialInstances;
};

REFLECT(Material, Resource)

END_PCCORE