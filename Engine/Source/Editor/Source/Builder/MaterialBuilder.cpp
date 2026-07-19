#include <Builder/MaterialBuilder.hpp>

#include <format>

#include <Log.hpp>
#include <ObjectPtr.hpp>
#include <Rendering/Material.hpp>
#include <Resources/ResourceManager.hpp>
#include <AssimpHelper.hpp>

namespace PC_EDITOR_CORE 
{
	MaterialBuilder::MaterialBuilder(Editor& _Editor)
		: m_Editor(_Editor)
	{

	}

    void MaterialBuilder::FillMaterialTexture(PC_CORE::Rendering::Material& CoreMaterial, 
                                              const aiMaterial& Material,
                                              const std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>>& TextureMap)
    {

        for (size_t j = 0; j < static_cast<size_t>(AI_TEXTURE_TYPE_MAX); j++)
        {
            const aiTextureType type = static_cast<aiTextureType>(j);

            switch (type)
            {
            case aiTextureType_NONE:
                continue;
            case aiTextureType_BASE_COLOR:// PBR albedo
            case aiTextureType_DIFFUSE:
            case aiTextureType_OPACITY:
                break;
            case aiTextureType_METALNESS: // metallic
                break;
            case aiTextureType_DIFFUSE_ROUGHNESS: // Rouhness
                break;
            case aiTextureType_NORMAL_CAMERA:
            case aiTextureType_NORMALS: // Normal
                break;
            case aiTextureType_EMISSION_COLOR:
            case aiTextureType_EMISSIVE: // Emisive
                break;
            case aiTextureType_LIGHTMAP: // AO
            case aiTextureType_AMBIENT_OCCLUSION:
                break;
            case aiTextureType_GLTF_METALLIC_ROUGHNESS:
            case aiTextureType_SPECULAR:
                break;
            case aiTextureType_HEIGHT:
            case aiTextureType_SHININESS:
            case aiTextureType_DISPLACEMENT:
            case aiTextureType_AMBIENT:
            case aiTextureType_REFLECTION:
            case aiTextureType_UNKNOWN:
            default:
                PC_LOG_VERBOSE("Ignore texture when build material {} type was {}", CoreMaterial.Name, AssimpTextureTypeToString(type).data());
                continue;
                break;
            }


            aiString textureName;

            const size_t TextureCount = Material.GetTextureCount(type);
            for (size_t k = 0; k < TextureCount; k++)
            {
                if (Material.GetTexture(type, k, &textureName) == aiReturn::aiReturn_SUCCESS)
                {
                    const auto& pair = TextureMap.at(std::string(textureName.C_Str())).second;
                    const auto Texture = pair.Lock();
                    if (!Texture)
                        continue;

                    if (type == aiTextureType_DIFFUSE)
                    {
                        CoreMaterial.SetAlbedoTexture(Texture);
                        if (Texture->Get()->UseAlpha())
                        {
                            CoreMaterial.SetMaterialType(PC_CORE::Rendering::MaterialDomain::Transparent);
                        }
                    }

                    if (type == aiTextureType_METALNESS ||
                        type == aiTextureType_DIFFUSE_ROUGHNESS ||
                        type == aiTextureType_GLTF_METALLIC_ROUGHNESS ||
                        type == aiTextureType_SPECULAR)
                    {
                        CoreMaterial.SetMetallicRoughnessAOTexture(Texture);
                    }

                    if (type == aiTextureType_AMBIENT_OCCLUSION || type == aiTextureType_LIGHTMAP)
                    {
                        CoreMaterial.SetAoTexture(Texture);
                    }

                    if (type == aiTextureType_NORMAL_CAMERA || type == aiTextureType_NORMALS)
                        CoreMaterial.SetNormalTexture(Texture);

                    if (type == aiTextureType_EMISSIVE || type == aiTextureType_EMISSION_COLOR)
                        CoreMaterial.SetEmissiveTexture(Texture);
                }
            }

        }
    }

	void MaterialBuilder::ResolveMaterials(const aiScene& _aiScene, 
                                           PC_CORE::StaticMesh& StaticMesh,
                                           const std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>>& TextureMap)
	{
        std::vector<PC_CORE::ObjectPtr<PC_CORE::Rendering::Material>> Materials;
        Materials.resize(_aiScene.mNumMaterials);

        for (size_t i = 0; i < Materials.size(); i++)
        {
            std::string matName;

            aiString str = _aiScene.mMaterials[i]->GetName();
            if (str.Empty())
            {  
                 matName = std::format("{} Material {}", StaticMesh.Name, std::to_string(i));
            }
            else
            {
                if (std::strcmp(str.C_Str(), "DefaultMaterial") == 0)
                {
                    matName = std::string(str.C_Str()) + "_" + StaticMesh.Name;
                }
                else
                {
                    matName = std::string(str.C_Str());
                }
            }

            if (PC_CORE::ResourceManager::Exist(matName))
            {
                Materials[i] = PC_CORE::ResourceManager::Get<PC_CORE::Rendering::Material>(matName);
            }
            else
            {
                Materials[i] = PC_CORE::ResourceManager::Create<PC_CORE::Rendering::Material>(matName);
            }

        }

        for (size_t i = 0; i < _aiScene.mNumMaterials; i++)
        {
            if (_aiScene.mMaterials[i] == nullptr)
                continue;

            aiMaterial& Material = *_aiScene.mMaterials[i];
            PC_CORE::Rendering::Material& CoreMaterial = *Materials[i];
            FillMaterialTexture(CoreMaterial, *_aiScene.mMaterials[i], TextureMap);

            {
                aiColor4D color;
                bool hasColor = false;

                // Prefer PBR base color
                if (Material.Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS)
                {
                    hasColor = true;
                }
                // Fallback to legacy diffuse
                else if (Material.Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                {
                    hasColor = true;
                }

                if (hasColor)
                {
                    CoreMaterial.SetAlbedoFactor(Tbx::Vector4f{
                        color.r, color.g, color.b, color.a
                        });
                }
            }

            float metallic = 0.0f;
            if (Material.Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS)
            {
                CoreMaterial.SetMetallicFactor(metallic);
            }

            float roughness = 1.0f;
            if (Material.Get(AI_MATKEY_METALLIC_FACTOR, roughness) == AI_SUCCESS)
            {
                CoreMaterial.SetRoughnessFactor(roughness);
            }

            float anisotropy = 1.0f;
            if (Material.Get(AI_MATKEY_ANISOTROPY_FACTOR, anisotropy) == AI_SUCCESS)
            {
                CoreMaterial.SetRoughnessFactor(anisotropy);
            }

            aiColor3D emmisive;
            if (Material.Get(AI_MATKEY_COLOR_EMISSIVE, emmisive) == AI_SUCCESS)
            {
                CoreMaterial.SetEmmisiveFactor(Tbx::Vector3f(emmisive.r, emmisive.g, emmisive.b));
            }

            float opacity = 1.0f;
            if (AI_SUCCESS == Material.Get(AI_MATKEY_OPACITY, opacity)) {
                if (opacity < 1.0f) {
                    // Material is transparent
                    CoreMaterial.SetMaterialType(PC_CORE::Rendering::MaterialDomain::Transparent);
                    CoreMaterial.SetUseAlpha(true);
                }
            }
        }

        // Build Only once
        std::set<PC_CORE::ObjectPtr<PC_CORE::Rendering::Material>> UniqueMaterial;
        for (auto& m : Materials)
            UniqueMaterial.emplace(m);

        for (auto& m : UniqueMaterial)
            m->Build();

        StaticMesh.SetBaseMaterials(Materials);
	}

		
} // PC_EDITOR_CORE
