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

        PC_CORE_API ~Material() override;

        DEFAULT_COPY_MOVE_OPERATIONS(Material)

        IMP_DYNAMIC_REFLECT();

        void Build();

        template <MaterialAttribute T, typename V>
        Material& SetMaterialAttribute(const V _Value)
        {
            m_MaterialAttributesValueType[static_cast<size_t>(T)] = MaterialValueTypeMap<V>::type;
            m_MaterialAttributesData[static_cast<size_t>(T)] = _Value;
        }

        // Setter
        /*
        template <MaterialAttribute MaterialAttribute, typename ValueDataType>
        Material& SetMaterialAttributeData(ValueDataType _ValueDataType)
        {
            m_MaterialAttributesValueType[static_cast<size_t>(MaterialAttribute)] = MaterialValueTypeMap<ValueDataType>::type;
            m_MaterialAttributesData[static_cast<size_t>(MaterialAttribute)] = _ValueDataType;
            return *this;
        }

        template<MaterialAttribute MaterialAttribute>
        Material& SetMaterialAttributeData(WeakObjectPtr<Texture2D>& _ValueDataType)
        {
            m_MaterialAttributesValueType[static_cast<size_t>(MaterialAttribute)] = MaterialValueType::TextureSample;
            m_MaterialAttributesData[static_cast<size_t>(MaterialAttribute)] = _ValueDataType;
        }

        Material& SetPackMetallicAndRougness(bool _Value)
        {
            m_UseMetallicRoughnessTexture = _Value;
        }


        // Getter

        template <MaterialAttribute MaterialAttribute>
        MaterialValueType GetMaterialAttributeValueType() const
        {
            return m_MaterialAttributesValueType[static_cast<size_t>(MaterialAttribute)];
        }

        bool GetPackMetallicAndRougness() const
        {
            return m_UseMetallicRoughnessTexture;
        }*/

    private:
        using MaterialAttributeData = 
            std::variant<std::monostate, 
            float, Tbx::Vector2f, 
            Tbx::Vector3f, Tbx::Vector4f, 
            std::pair<WeakObjectPtr<const Texture2D>, WeakObjectPtr<const Sampler>>
            >;

        MaterialType MaterialType = MaterialType::Opaque;

        std::array<MaterialValueType, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_MaterialAttributesValueType;

        std::array<MaterialAttributeData, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_MaterialAttributesData;

        std::unique_ptr<RhiDescriptorSet> m_RhiDescriptorSets = nullptr;
    };

    REFLECT(Material, Resource)

}

using MaterialAttribute = PC_CORE::Rendering::MaterialAttribute;
