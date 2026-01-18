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
        MetallicRoughnessAnisotropy,
        Normal,
        Emisive,
        Ao,
    };
 
    namespace Gpu
    {
        struct GPU_ALIGN MaterialBuffer
        {
            std::array<int, 8> TextureDescriptor = {0};
            Gpu::vec4 Albedo = {0.f};
            Gpu::vec4 MetallicRoughnessAnisotropy{ 0.f };
            Gpu::vec3 Emissive{ 0.f };
            float Pad01 = 0xDEAD;
            float AO = 0.f;
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

        Material& SetAlbedo(const Tbx::Vector4f& _Albedo)
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

        Material& GetAmbiantOcclusion(float _AmbiantOcclusion)
        {
            m_AmbiantOcclusion = _AmbiantOcclusion;
            return *this;
        }

        Material& SetAlbedoTexture(const ObjectPtr<Texture2D>& _AlbedoTexture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Albedo)] = _AlbedoTexture;
            return *this;
        }

        Material& SetMetallicRougnessAnisotropyTexture(const ObjectPtr<Texture2D>& _MetallicSpecularRougnessTextureTextureAnisotropy)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::MetallicRoughnessAnisotropy)] = _MetallicSpecularRougnessTextureTextureAnisotropy;
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

        PC_CORE_API void Upload();

        const Tbx::Vector4f& GetAlbedo() const
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

        float GetAmbiantOcclusion() const
        {
            return m_AmbiantOcclusion;
        }

        const RhiDescriptorSet* GetDescriptorSet() const;

        size_t GetMaterialStride() const;
    private:

        std::array<WeakObjectPtr<Texture2D>, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_Textures;

        Tbx::Vector4f m_Albedo = Tbx::Vector4f(0.f, 0.f, 0.f, 1.0f);

        float m_Metallic = 0.f;

        float m_Specular = 0.5f;

        float m_Roughness = 0.5f;

        float m_Anisotropy = 0.f;

        Tbx::Vector3f m_Emmisive = Tbx::Vector3f(0.f, 0.f, 0.f);

        float m_AmbiantOcclusion = 0.f;

        MaterialType MaterialType = MaterialType::Opaque;

        std::unique_ptr<RhiDescriptorSet> m_RhiDescriptorSets = nullptr;

        std::unique_ptr<RhiBuffer> m_RhiMaterialBuffer = nullptr;

        void PopulateGpuMaterial(Gpu::MaterialBuffer& _MaterialBuffer);
    };

    REFLECT(Material, Resource)

}

using MaterialAttribute = PC_CORE::Rendering::MaterialAttribute;
