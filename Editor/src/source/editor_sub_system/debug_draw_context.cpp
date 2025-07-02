#include "editor_sub_system/debug_draw_context.hpp"

#include "editor.hpp"
#include "resources/resource_manager.hpp"


void PC_EDITOR_CORE::DebugDrawContext::DrawSphere(Tbx::Vector3f _p1, float _radius)
{
    
}

void PC_EDITOR_CORE::DebugDrawContext::OnRender()
{
    EditorSubSystem::OnRender();
    
}

PC_EDITOR_CORE::DebugDrawContext::DebugDrawContext(Editor& _editor) : EditorSubSystem(_editor)
{
    
    m_Instance = this;
    
    m_Editor.gameApp.renderer.UserCustomForwardPass.push_back(
     std::bind(&PC_EDITOR_CORE::DebugDrawContext::DrawDebugPrimitive, this,
               std::placeholders::_1, std::placeholders::_2)
 );
    CreateShaders();
    m_GizmoShader.lock()->AllocDescriptorSet(&m_DescriptorSet, 0);

    
    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
   {
        {
            PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
            CAMERA_BINDING,
            & cameraBufferDescritptor,
            nullptr,
        },
        
   };

}

void PC_EDITOR_CORE::DebugDrawContext::CreateShaders()
{
    
    constexpr PC_CORE::RasterizerInfo rasterizerInfo =
      {
        .polygonMode = PC_CORE::PolygonMode::Fill,
        .cullModeFlag = PC_CORE::CullModeFlagBit::Back,
        .frontFace = PC_CORE::FrontFace::CounterClockwise
    };

    PC_CORE::VertexInputBindingDescrition vertexInputBindingInstance =
        {
        .binding = 1,
        .stride = ,
        .vertexInputRate = vk::VertexInputRate::eInstance
        }
    

    const PC_CORE::ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo = rasterizerInfo,
        .vertexInputBindingDescritions = {PC_CORE::Vertex::GetBindingDescrition(0)},
        .vertexAttributeDescriptions = PC_CORE::Vertex::GetAttributeDescriptions(0),
        .enableDepthTest = true,
    };


    const std::vector<std::pair<PC_CORE::ShaderStageType, std::string>> source =
 {
        {
            PC_CORE::ShaderStageType::VERTEX,
            "debug_draw_spv.vert"
        },  
        {
            PC_CORE::ShaderStageType::FRAGMENT,
            "debug_draw_spv.frag"
        }
 };

    PC_CORE::ShaderInfo shaderInfo =
        {
        .shaderProgramPipelineType = PC_CORE::ShaderProgramPipelineType::POINT_GRAPHICS,
        .shaderInfoData = shaderGraphicPointInfo,
        .shaderSources = source
        } ;
    
    
    PC_CORE::ProgramShaderCreateInfo _programShaderCreateInfo =
        {
        .shaderInfo = shaderInfo,
        .renderPass = m_Editor.gameApp.renderer.forwardPass,
        };

    m_GizmoShader = PC_CORE::ResourceManager::Create<PC_CORE::ShaderProgram>();
}

void PC_EDITOR_CORE::DebugDrawContext::DrawDebugPrimitive(PC_CORE::CommandList* _commandList,
                                                          const PC_CORE::RenderingContext& _renderingContext)
{
    if (_renderingContext.renderingContextFlag & PC_CORE::RenderingContextFlag::DebugDrawGeometry)
        return;


    
    
    
}
