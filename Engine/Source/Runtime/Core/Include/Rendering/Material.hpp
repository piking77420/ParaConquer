#pragma once

#include "CoreHeader.hpp"
#include "MaterialInstance.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "Resources/Texture2d.hpp"
#include "ObjectPtr.hpp"
#include "ShaderProgram.hpp"

BEGIN_PCCORE
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
        MaterialType MaterialType = MaterialType::Opaque;

        WeakObjectPtr<Texture2D> Albedo;

        WeakObjectPtr<Texture2D> Metallic;

        WeakObjectPtr<Texture2D> Roughess;

        WeakObjectPtr<Texture2D> Normal;


        static std::shared_ptr<MaterialInstance> CreateMaterialInstance();

        PC_CORE_API IMP_DYNAMIC_REFLECT();

        PC_CORE_API Material();

        PC_CORE_API explicit Material(const std::string& _name);

        PC_CORE_API ~Material() override;

        PC_CORE_API void Build();

        const ShaderProgramDescriptorSets* GetDescriptorSet() const
        {
            return m_PShaderProgramDescriptorSets;
        }

    private:
        ShaderProgramDescriptorSets* m_PShaderProgramDescriptorSets = nullptr;

        WeakObjectPtr<ShaderProgram> m_ShaderProgram;

        std::vector<std::shared_ptr<MaterialInstance>> m_MaterialInstances;
    };

    REFLECT(Material, Resource)

END_PCCORE
