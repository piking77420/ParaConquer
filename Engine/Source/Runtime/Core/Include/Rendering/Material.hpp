#pragma once

#include <type_traits>
#include <array>

#include "RenderingTypedef.h"
#include "ObjectPtr.hpp"
#include "Resources/Texture2d.hpp"
#include <Rendering/MaterialDomain.hpp>
#include <Rendering/MeshPass.hpp>
#include <Rendering/PipelineStateObject.hpp>
#include <LowRenderer/RhiBuffer.h>
#include <LowRenderer/RhiDescriptorSet.hpp>


namespace PC_CORE::Rendering
{

    enum struct MaterialAttribute : uint8_t
    {
        Albedo,
        Normal,
        Emisive,
        AoRoughnessMetallic,
        Ao,
    };
 
    namespace Gpu
    {
        struct GPU_ALIGN MaterialBuffer
        {
            std::array<int, 4> TextureDescriptor = {0};  // AlbedoTexture // Metallic // Emmisive // ORM
            std::array<int, 4> TextureDescriptor2 = { 0 }; // AO
            Gpu::vec4 AlbedoFactors = {0.f};
            Gpu::vec4 AORoughnessMetallicFactors{ 0.f };
            Gpu::vec3 EmissiveFactor{ 0.f };
            int UseAlpha = 0;
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

        Material& SetMaterialType(const MaterialDomain _Type)
        {
            m_MaterialDomain = _Type;
            return *this;
        }

        Material& SetAlbedoFactor(const Tbx::Vector4f& _Albedo)
        {
            m_Albedo = _Albedo;
            return *this;
        }

        Material& SetMetallicFactor(float _Metallic)
        {
            m_Metallic = _Metallic;
            return *this;
        }

        Material& SetRoughnessFactor(float _Roughness)
        {
            m_Roughness = _Roughness;
            return *this;
        }

        Material& SetAnisotropyFactor(float _Anisotropy)
        {
            m_Anisotropy = _Anisotropy;
            return *this;
        }

        Material& SetEmmisiveFactor(const Tbx::Vector3f& _Emmisive)
        {
            m_Emmisive = _Emmisive;
            return *this;
        }

        Material& SetAmbiantOcclusion(float _AmbiantOcclusion)
        {
            m_AmbiantOcclusion = _AmbiantOcclusion;
            return *this;
        }

        Material& SetUseAlpha(bool _UseAlpha)
        {
            m_UseAlpha = _UseAlpha;
            return *this;
        }

        Material& SetAlbedoTexture(const ObjectPtr<Texture2D>& _AlbedoTexture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Albedo)] = _AlbedoTexture;
            return SetUseAlpha(_AlbedoTexture->Get()->UseAlpha());
        }

        Material& SetMetallicRoughnessAOTexture(const ObjectPtr<Texture2D>& _MetallicSpecularRougnessTextureTextureAnisotropy)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::AoRoughnessMetallic)] = _MetallicSpecularRougnessTextureTextureAnisotropy;
            return *this;
        }

        Material& SetNormalTexture(const ObjectPtr<Texture2D>& _Texture)
        {
            m_Textures[static_cast<size_t>(MaterialAttribute::Normal)] = _Texture;
            return *this;
        }


        Material& SetEmissiveTexture(const ObjectPtr<Texture2D>& _Texture)
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

        MaterialDomain GetMaterialDomain() const
        {
            return m_MaterialDomain;
        }

        const Tbx::Vector4f& GetAlbedo() const
        {
            return m_Albedo;
        }

        float GetMetallic() const
        {
            return m_Metallic;
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

        bool GetUseAlpha() const
        {
            return m_UseAlpha;
        }

        const RhiDescriptorSet* GetDescriptorSet() const;

        size_t GetMaterialStride() const;

        template <typename Self>
        [[nodiscard]] auto GetPipelineStateObject(this Self&& self, MeshPass _Pass) noexcept
        {
            assert(_Pass != MeshPass::Count);

            return std::forward<Self>(self).m_PipelineData[static_cast<std::size_t>(_Pass)];
        }

        std::array<PipelineStateObject, static_cast<size_t>(MeshPass::Count)>& GetPso()
        {
            return m_PipelineData;
        }
    private:
        MaterialDomain m_MaterialDomain = MaterialDomain::Opaque;

        std::array<WeakObjectPtr<Texture2D>, static_cast<size_t>(MaterialAttribute::Ao) + 1> m_Textures;

        Tbx::Vector4f m_Albedo = Tbx::Vector4f(1.f, 1.f, 1.f, 1.0f);

        float m_Metallic = 1.f;

        float m_Roughness = 1.0f;

        float m_Anisotropy = 0.f;

        Tbx::Vector3f m_Emmisive = Tbx::Vector3f(0.f, 0.f, 0.f);

        float m_AmbiantOcclusion = 1.f;

        bool m_UseAlpha = false;

        std::unique_ptr<RhiDescriptorSet> m_RhiDescriptorSets = nullptr;

        std::unique_ptr<RhiBuffer> m_RhiMaterialBuffer = nullptr;

        std::array<PipelineStateObject, static_cast<size_t>(MeshPass::Count)> m_PipelineData;

        void PopulateGpuMaterial(Gpu::MaterialBuffer& _MaterialBuffer);

        void WritePso();
    };

    REFLECT(Material, Resource)

}

using MaterialAttribute = PC_CORE::Rendering::MaterialAttribute;
