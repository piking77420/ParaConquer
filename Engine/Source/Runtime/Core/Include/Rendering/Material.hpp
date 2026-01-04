#pragma once

#include <type_traits>
#include <array>

#include "RenderingTypedef.h"
#include "ObjectPtr.hpp"
#include "Resources/Texture2d.hpp"
#include "LowRenderer/RhiDescriptorSet.hpp"

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

    template<typename T>
    struct MaterialValueTypeMap;

    template<>
    struct MaterialValueTypeMap<float>
    {
        MaterialValueType type = MaterialValueType::Scalar;
    };

    template<>
    struct MaterialValueTypeMap<Tbx::Vector2f>
    {
        MaterialValueType type = MaterialValueType::Scalar;
    };

    template<>
    struct MaterialValueTypeMap<Tbx::Vector3f>
    {
        MaterialValueType type = MaterialValueType::Scalar;
    };

    template<>
    struct MaterialValueTypeMap<Tbx::Vector4f>
    {
        MaterialValueType type = MaterialValueType::Scalar;
    };

    template<>
    struct MaterialValueTypeMap<bool>
    {
        MaterialValueType type = MaterialValueType::Scalar;
        
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
    concept ValueDataType = std::is_same_v<T, float> || std::is_same_v<T, std::array<float, 2>> || 
        std::is_same_v<T, std::array<float, 3>> || std::is_same_v<T, std::array<float, 4>> || std::is_same_v<T, WeakObjectPtr<Texture2D>> || std::is_same_v<T, ObjectPtr<Texture2D>>;

    class Material : public Resource
    {
    public:
        PC_CORE_API Material();

        PC_CORE_API ~Material() override;

        DEFAULT_COPY_MOVE_OPERATIONS(Material)

        IMP_DYNAMIC_REFLECT();

        void Build();

        // Setter

        template <MaterialAttribute MaterialAttribute, typename ValueDataType>
        Material& SetMaterialAttributeData(ValueDataType _ValueDataType)
        {
            m_MaterialAttributesValueType[static_cast<size_t>(MaterialAttribute)] = MaterialValueTypeMap<ValueDataType>::type;
            m_MaterialAttributesData[static_cast<size_t>(MaterialAttribute)] = _ValueDataType;
            return *this;
        }

        // Getter

        template <MaterialAttribute MaterialAttribute>
        MaterialValueType GetMaterialAttributeValueType() const
        {
            return m_MaterialAttributesValueType[static_cast<size_t>(MaterialAttribute)];
        }

       

    private:
        std::unique_ptr<RhiDescriptorSet> m_RhiDescriptorSets = nullptr;

        using MaterialAttributeData = std::variant<std::monostate, float, Tbx::Vector2f, Tbx::Vector3f, Tbx::Vector4f, WeakObjectPtr<Texture2D>>;

        MaterialType MaterialType = MaterialType::Opaque;

        std::array<MaterialValueType, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_MaterialAttributesValueType;

        std::array<MaterialAttributeData, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_MaterialAttributesData;

    };

    REFLECT(Material, Resource)

}

using MaterialAttribute = PC_CORE::Rendering::MaterialAttribute;
