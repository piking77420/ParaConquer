#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <future>

#include "EditorHeader.hpp"
#include "ObjectPtr.hpp"
#include "LowRenderer/RhiTexture.hpp"
#include "LowRenderer/RhiResourceUpdate.hpp"
#include "Resources/StaticMesh.hpp"
#include "Resources/Texture2D.hpp"

namespace PC_CORE
{
    class Serializer;
    class Rhi;
    struct StaticMeshRenderData;
    class Image;
}

namespace PC_CORE::Thread
{
    class ThreadPool;
}

namespace PC_CORE::Rendering
{
    class Material;
}

struct aiScene;
struct aiTexture;
enum aiTextureType;

BEGIN_EDITOR_PCCORE
    class AssetsImporter
    {
    public:
        [[nodiscard]] bool ImportModel(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, const std::filesystem::path& _path);

        const std::string& GetName() const;

        const std::vector<PC_CORE::ObjectPtr<PC_CORE::StaticMesh>>& GetStaticMeshes() const
        {
            return m_StaticMeshs;
        }


    private:
        enum class ImportFormat
        {
            None,
            Gltf,
            Fbc,
            Obj
        };

        bool m_Succes = false;

        std::string m_ImportObjectName;

        std::filesystem::path m_filePath;

        std::vector<PC_CORE::ObjectPtr<PC_CORE::StaticMesh>> m_StaticMeshs;

        std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>> m_TextureMaps;

        std::vector<PC_CORE::Rendering::Material> m_Materials;

        std::vector<PC_CORE::RHI::ResourceUpdateBranch> m_ResourceUpdateBranchs;

        std::mutex m_mutex;

        ImportFormat m_ImportFormat;

        ImportFormat FindImportFormat(const std::filesystem::path& path);

        bool ImportMeshesFromScene(PC_CORE::Rhi& _Rhi, const aiScene* scene);

        bool ImportTextures(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, std::vector<std::future<void>>* Futures, const aiScene* scene);

        bool ImportMaterials(PC_CORE::Rhi& _Rhi, const aiScene* scene);

        [[nodiscard]] PC_CORE::RhiTexture* RhiTextureFromAiTexture(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture);

        [[nodiscard]] PC_CORE::RhiTexture* RhiTextureFromPath(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture);

        [[nodiscard]] void BuildRhiTextureFromImage(PC_CORE::Rhi& _Rhi, PC_CORE::RhiTexture& _Texture, PC_CORE::Image* _Image);

        //bool ImportMaterial(const aiScene* scene, std::vector<PC_CORE::Rendering::Material>* _Material);

        //bool ImportMaterialGltf(const aiScene* scene, std::vector<PC_CORE::Rendering::Material>* _Material);

    };

END_EDITOR_PCCORE
