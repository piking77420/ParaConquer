#pragma once
#include <string>
#include <unordered_map>

#include "EditorHeader.hpp"
#include "ObjectPtr.hpp"

namespace PC_CORE
{
    class Serializer;
    class Rhi;
    class StaticMesh;
    struct StaticMeshRenderData;
}

namespace PC_CORE::Rendering
{
    //class Material;
}

class aiScene;

BEGIN_EDITOR_PCCORE
    class AssetsImporter
    {
    public:
        [[nodiscard]] bool ImportModel(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::StaticMesh* _StaticMesh);

    private:
        enum class ImportFormat
        {
            None,
            Gltf,
            Fbc,
            Obj
        };

        ImportFormat m_ImportFormat;

        ImportFormat FindImportFormat(const std::filesystem::path& path);

        bool ImportMeshesFromScene(const aiScene* scene, PC_CORE::StaticMeshRenderData& _StaticMeshRenderData);

       // bool ImportMaterial(const aiScene* scene, std::vector<PC_CORE::Rendering::Material>* _Material);

    };

END_EDITOR_PCCORE
