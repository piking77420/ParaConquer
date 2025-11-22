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

        const ShaderProgramDescriptorSets* GetDescriptorSet() const
        {
            return m_PShaderProgramDescriptorSets.get();
        }

    private:
        std::unique_ptr<ShaderProgramDescriptorSets> m_PShaderProgramDescriptorSets = nullptr;

        RhiShaderProgram* m_ShaderProgram{ nullptr };

        std::vector<std::shared_ptr<MaterialInstance>> m_MaterialInstances;
    };

    REFLECT(Material, Resource)

END_PCCORE
