#pragma once

#include "editor_header.hpp"
#include "hierachy.hpp"
#include "low_renderer/command_list.hpp"
#include "rendering/renderer.hpp"
#include "resources/graphic_shader.hpp"

BEGIN_EDITOR_PCCORE
    class Editor;
// icon drawer 
class EditorRenderer
{
public:
    EditorRenderer(Editor& _editor);
    
    EditorRenderer() = default;

    ~EditorRenderer() = default;

    void PushCustomCommand();
    
    void DrawSelectedEntity();
private:
    Editor* m_Editor = nullptr;

    std::weak_ptr<PC_CORE::GraphicShader> m_DrawSpriteShader;

    std::weak_ptr<PC_CORE::Texture2D> m_DirectionalLightTexture;
    
    std::weak_ptr<PC_CORE::Texture2D> m_SpotLightTexture;
    
    std::weak_ptr<PC_CORE::Texture2D> m_PointLightTexture;

    PC_CORE::ShaderProgramDescriptorSets* m_CameraSet = nullptr;

    PC_CORE::ShaderProgramDescriptorSets* m_DirectionalDescriptorSet = nullptr;

    PC_CORE::ShaderProgramDescriptorSets* m_SpotLightDescriptorSet = nullptr;

    PC_CORE::ShaderProgramDescriptorSets* m_PointLightDescriptorSet = nullptr;

    std::vector<size_t> m_DirectionalLightIndices;
    std::vector<size_t> m_SpotLightIndices;
    std::vector<size_t> m_PointLightIndices;
    
    void DrawLightGizmo(PC_CORE::Renderer& _renderer, PC_CORE::CommandList* _commandList,
        const PC_CORE::RenderingContext& _renderingContext, const PC_CORE::RenderingWorldData* _renderingWorldData);

    void InitResources();
};

END_EDITOR_PCCORE