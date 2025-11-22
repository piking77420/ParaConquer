#pragma once

#include "EditorHeader.hpp"
#include "Hierachy.hpp"
#include "LowRenderer/CommandList.hpp"
#include "Rendering/Renderer.hpp"
#include "Resources/GraphicShader.hpp"

BEGIN_EDITOR_PCCORE
    class Editor;

    // icon drawer 
    class EditorRenderer
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(EditorRenderer)
        
        EditorRenderer(Editor& _editor);

        EditorRenderer() = default;

        ~EditorRenderer() = default;

        void PushCustomCommand();

        void DrawSelectedEntity();

    private:
        Editor* m_Editor = nullptr;

        std::unique_ptr<PC_CORE::RhiShaderProgram> m_DrawSpriteShader;

        std::weak_ptr<PC_CORE::Texture2D> m_DirectionalLightTexture;

        std::weak_ptr<PC_CORE::Texture2D> m_SpotLightTexture;

        std::weak_ptr<PC_CORE::Texture2D> m_PointLightTexture;

        std::unique_ptr<PC_CORE::ShaderProgramDescriptorSets> m_CameraSet = nullptr;

        std::unique_ptr<PC_CORE::ShaderProgramDescriptorSets> m_DirectionalDescriptorSet = nullptr;

        std::unique_ptr<PC_CORE::ShaderProgramDescriptorSets> m_SpotLightDescriptorSet = nullptr;

        std::unique_ptr<PC_CORE::ShaderProgramDescriptorSets> m_PointLightDescriptorSet = nullptr;

        std::vector<size_t> m_DirectionalLightIndices;
        std::vector<size_t> m_SpotLightIndices;
        std::vector<size_t> m_PointLightIndices;

        PC_CORE::Sampler m_SpriteSampler;

        void DrawLightGizmo(PC_CORE::Renderer& _renderer, PC_CORE::CommandList* _commandList,
                            const PC_CORE::RenderingContext& _renderingContext,
                            const PC_CORE::RenderingWorldData* _renderingWorldData);

        void InitResources();
    };

END_EDITOR_PCCORE
