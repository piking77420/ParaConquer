#pragma once

#include <string_view>

#include <EditorHeader.hpp>

#include <Resources/StaticMesh.hpp>
#include <assimp/material.h>

struct aiScene;
struct aiMaterial;

namespace PC_EDITOR_CORE 
{
    class Editor;

    class MaterialBuilder
    {
    public:
        MaterialBuilder(Editor& _Editor);

        ~MaterialBuilder() = default;

        void ResolveMaterials(const aiScene& _aiScene,
                              PC_CORE::StaticMesh& StaticMesh,
                              const std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>>& TextureMap);

    private:
        Editor& m_Editor;

        void FillMaterialTexture(PC_CORE::Rendering::Material& CoreMaterial, 
                                 const aiMaterial& Material,
                                 const std::map<std::string, std::pair<aiTextureType, PC_CORE::WeakObjectPtr<PC_CORE::Texture2D>>>& TextureMap);
    };

} // PC_EDITOR_CORE
