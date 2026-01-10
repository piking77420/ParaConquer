#pragma once

#include <type_traits>
#include <array>

#include "RenderingTypedef.h"
#include "ObjectPtr.hpp"
#include "Resources/Texture2d.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"
#include "Sampler.hpp"

namespace PC_CORE::Rendering
{

    enum class MaterialType
    {
        Opaque,
        Transparent,
    };

    enum struct MaterialAttribute : uint8_t
    {
        Color,
        Metallic,
        Roughness,
        Normal,
        Ao,
        MetallicRougnessPack,
    };

    enum struct MaterialValueType : uint8_t
    {
        Scalar,
        Vec2,
        Vec3,
        Vec4,
        TextureSample,
    };

    using TextureSampler = std::pair<const ObjectPtr<const Texture2D>, const ObjectPtr<const Sampler>>;

    template<typename T>
    struct MaterialValueTypeMap;

    template<>
    struct MaterialValueTypeMap<float>
    {
        static constexpr MaterialValueType type = MaterialValueType::Scalar;
    };

    template<>
    struct MaterialValueTypeMap<Tbx::Vector2f>
    {
        static constexpr MaterialValueType type = MaterialValueType::Vec2;
    };

    template<>
    struct MaterialValueTypeMap<Tbx::Vector3f>
    {
        static constexpr MaterialValueType type = MaterialValueType::Vec3;
    };

    template<>
    struct MaterialValueTypeMap<Tbx::Vector4f>
    {
        static constexpr MaterialValueType type = MaterialValueType::Vec4;
    };

    template<>
    struct MaterialValueTypeMap<TextureSampler>
    {
        static constexpr MaterialValueType type = MaterialValueType::TextureSample;
    };

    
    namespace Gpu
    {
        struct GPU_ALIGN MaterialBuffer
        {
            std::array<MaterialValueType, static_cast<size_t>(MaterialAttribute::Ao) + 1> MaterialValueTypes;
            uint8_t reserved[3];
            uint8_t pad0[4];

            uint32_t MaterialId;
            std::array<vec4, static_cast<size_t>(MaterialAttribute::Ao) + 1> MaterialData;
        };
    }


    template<class T>
    concept ValueDataType = std::is_same_v<T, float> || std::is_same_v<T, Tbx::Vector2f> ||
        std::is_same_v<T, Tbx::Vector3f> || std::is_same_v<T, Tbx::Vector4f> || std::is_same_v<T, TextureSampler>;


    class Material : public Resource
    {
    public:
        PC_CORE_API Material();

        PC_CORE_API Material(std::string _Name);

        PC_CORE_API ~Material() override;

        DEFAULT_COPY_MOVE_OPERATIONS(Material)

        IMP_DYNAMIC_REFLECT();

        PC_CORE_API void Build();

        RhiDescriptorSet* GetDescriptorSet();

        WeakObjectPtr<Texture2D> m_Albedo;

    private:

        MaterialType MaterialType = MaterialType::Opaque;

        std::unique_ptr<RhiDescriptorSet> m_RhiDescriptorSets = nullptr;

        std::unique_ptr<RhiSampler> TextureSampler;
    };

    REFLECT(Material, Resource)

}

using MaterialAttribute = PC_CORE::Rendering::MaterialAttribute;
