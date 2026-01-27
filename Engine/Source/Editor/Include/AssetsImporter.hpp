#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <future>
#include <utility>

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
struct aiMaterial;

BEGIN_EDITOR_PCCORE
    class AssetsImporter
    {
    public:
        [[nodiscard]] bool ImportModel(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, const std::filesystem::path& _path);

        const std::string& GetName() const;

        const PC_CORE::ObjectPtr<PC_CORE::StaticMesh>& GetStaticMeshes() const
        {
            return m_StaticMeshs;
        }

        const std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>>& GetTextures() const
        {
            return m_TextureMaps;
        }

        bool GetSuccess() const
        {
            return m_Succes;
        }

        const PC_CORE::Guid& GetGuid() const
        {
            return m_Guid;
        }

        bool operator==(const AssetsImporter& AssetsImporter) const
        {
            return m_Guid == AssetsImporter.m_Guid;
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

        ImportFormat m_ImportFormat;

        PC_CORE::Guid m_Guid = PC_CORE::Guid::New();

        std::mutex m_mutex;

        std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>> m_TextureMaps;

        PC_CORE::ObjectPtr<PC_CORE::StaticMesh> m_StaticMeshs;

        std::unordered_map<size_t, std::vector<std::string>> m_MaterialsMap;

        std::vector<PC_CORE::RHI::ResourceUpdateBranch> m_ResourceUpdateBranchs;

        ImportFormat FindImportFormat(const std::filesystem::path& path);

        bool ImportMeshesFromScene(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, const aiScene* scene);

        bool ImportTextures(PC_CORE::Rhi& _Rhi, PC_CORE::Thread::ThreadPool& ThreadPool, std::vector<std::future<void>>* Futures, const aiScene* scene);

        void ResolveMaterial(const aiScene* scene);

        void FillMaterialTexture(PC_CORE::Rendering::Material& CoreMaterial, const aiMaterial& Material);

        std::pair<std::vector<uint32_t>, std::vector<PC_CORE::StaticMeshVertex>> OptimiseMesh(
            PC_CORE::SubMesh& SubMesh,
            const std::span<const PC_CORE::StaticMeshVertex>& UnOptVertices,
            const std::span<const uint32_t>& UnOptIndices);

        void BuildMeshlet(PC_CORE::StaticMeshRenderData* _StaticMeshRenderData, PC_CORE::SubMesh::MeshletData& _SubMeshMeshletData, const std::span<uint32_t>& _Indices);

        void LoadMesh(PC_CORE::Thread::ThreadPool& ThreadPool, PC_CORE::StaticMeshRenderData* _StaticMeshRenderData, const aiScene* scene);

        [[nodiscard]] PC_CORE::RhiTexture* RhiTextureFromAiTexture(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture, aiTextureType textureType);

        [[nodiscard]] PC_CORE::RhiTexture* RhiTextureFromPath(PC_CORE::Rhi& _Rhi, const char* TextureName, const aiTexture& aiTexture);

        [[nodiscard]] void BuildRhiTextureFromImage(PC_CORE::Rhi& _Rhi, PC_CORE::RhiTexture& _Texture, PC_CORE::Image* _Image, aiTextureType textureType , bool _UseApha);


    };

END_EDITOR_PCCORE
