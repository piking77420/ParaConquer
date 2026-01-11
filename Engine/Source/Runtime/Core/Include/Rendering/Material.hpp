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
        Albedo,
        MetallicSpecularRoughnessAnisotropy,
        Normal,
        Emisive,
        Ao,
    };
 
    namespace Gpu
    {
        struct GPU_ALIGN MaterialBuffer
        {
            std::array<int, 4> TextureDescriptor; // bit flag for if attrivute has texture
            Gpu::vec4 Albedo;
            float Pad0;
            Gpu::vec4 MetallicSpecularRoughnessAnisotropy;
            Gpu::vec3 Emissive;
            float Pad01;
            float AO;
        };
    }


    class Material : public Resource
    {
    public:
        PC_CORE_API Material();

        PC_CORE_API Material(std::string _Name);

        PC_CORE_API ~Material() override;

        DEFAULT_COPY_MOVE_OPERATIONS(Material)

        IMP_DYNAMIC_REFLECT();

        PC_CORE_API void Build();

        Material& SetAlbedo(const Tbx::Vector3f& _Albedo)
        {
            m_Albedo = _Albedo;
            return *this;
        }

        Material& SetMetallic(float _Metallic)
        {
            m_Metallic = _Metallic;
            return *this;
        }

        Material& SetSpecular(float _Specular)
        {
            m_Specular = _Specular;
            return *this;
        }

        Material& SetRoughness(float _Roughness)
        {
            m_Roughness = _Roughness;
            return *this;
        }

        Material& SetAnisotropy(float _Anisotropy)
        {
            m_Anisotropy = _Anisotropy;
            return *this;
        }

        Material& SetEmmisive(const Tbx::Vector3f& _Emmisive)
        {
            m_Emmisive = _Emmisive;
            return *this;
        }

        Material& SetAlbedoTexture(const ObjectPtr<Texture2D>& _AlbedoTexture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Albedo)] = _AlbedoTexture;
            return *this;
        }

        Material& SetMetallicSpecularRougnessAnisotropyTexture(const ObjectPtr<Texture2D>& _MetallicSpecularRougnessTextureTextureAnisotropy)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::MetallicSpecularRoughnessAnisotropy)] = _MetallicSpecularRougnessTextureTextureAnisotropy;
            return *this;
        }

        Material& SetNormalTexture(const ObjectPtr<Texture2D>& _Texture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Normal)] = _Texture;
            return *this;
        }

        Material& SetEmisiveTexture(const ObjectPtr<Texture2D>& _Texture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Emisive)] = _Texture;
            return *this;
        }

        Material& SetAoTexture(const ObjectPtr<Texture2D>& _Texture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Ao)] = _Texture;
            return *this;
        }

        const Tbx::Vector3f& GetAlbedo() const
        {
            return m_Albedo;
        }

        float GetMetallic() const
        {
            return m_Metallic;
        }

        float GetSpecular() const
        {
            return m_Specular;
        }

        float GetRoughness() const
        {
            return m_Roughness;
        }

        float GetAnisotropy() const
        {
            return m_Anisotropy;
        }

        const Tbx::Vector3f& GetEmmisive() const
        {
            return m_Emmisive;
        }

        RhiDescriptorSet* GetDescriptorSet();
    private:

        std::array<WeakObjectPtr<Texture2D>, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_Textures;

        Tbx::Vector3f m_Albedo = Tbx::Vector3f(0.f, 0.f, 0.f);

        float m_Metallic = 0.f;

        float m_Specular = 0.5f;

        float m_Roughness = 0.5f;

        float m_Anisotropy = 0.f;

        Tbx::Vector3f m_Emmisive = Tbx::Vector3f(0.f, 0.f, 0.f);

        MaterialType MaterialType = MaterialType::Opaque;

        std::unique_ptr<RhiDescriptorSet> m_RhiDescriptorSets = nullptr;

        std::unique_ptr<RhiBuffer> m_RhiMaterialBuffer = nullptr;

    };

    REFLECT(Material, Resource)

}

using MaterialAttribute = PC_CORE::Rendering::MaterialAttribute;
