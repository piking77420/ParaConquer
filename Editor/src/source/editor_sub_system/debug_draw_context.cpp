#include "editor_sub_system/debug_draw_context.hpp"

#include "editor.hpp"
#include "resources/resource_manager.hpp"


void PC_EDITOR_CORE::DebugDrawContext::DrawSphere(Tbx::Vector3d _p1, float _radius, Tbx::Vector4f _color)
{
    Tbx::Vector3f p1 = _p1;
    
    const Tbx::Matrix4x4f matrix = Tbx::Matrix4x4f
    (       _radius, 0.f, 0.f, 0.f,
            0.f, _radius, 0.f, 0.f,
            0.f, 0.f, _radius, 0.f,
            p1.x, p1.y, p1.z, 1.f
    );

    m_Instance->m_SphereGizmoBuffers.push_back(matrix);
}

void PC_EDITOR_CORE::DebugDrawContext::Render()
{
    EditorSubSystem::Render();
    m_SpherePrimitiveGpuResources.vertexBuffer.Update(m_Instance->m_SphereGizmoBuffers.data(),
        m_Instance->m_SphereGizmoBuffers.size() * sizeof(Tbx::Matrix4x4f));
    
    m_SphereGizmoBuffers.clear();
}

PC_EDITOR_CORE::DebugDrawContext::DebugDrawContext(Editor& _editor) : EditorSubSystem(_editor)
{
    m_Instance = this;
    
    m_Editor.gameApp.renderer.UserCustomForwardPass.push_back(
     std::bind(&PC_EDITOR_CORE::DebugDrawContext::DrawDebugPrimitive, this,
               std::placeholders::_1, std::placeholders::_2));
    /*
    CreateShaders();
    m_ShaderProgram.lock()->AllocDescriptorSet(&m_SpherePrimitiveGpuResources.descriptorSet, 0);
*/
    m_SpherePrimitiveGpuResources.vertexBuffer = PC_CORE::VertexBuffer(sizeof(GpuBufferGizmo), PC_CORE::MemoryLocalisation::CPU_Only, PC_CORE::MemoryUsage::Dynamic);
}

void PC_EDITOR_CORE::DebugDrawContext::CreateShaders()
{
    /*
    constexpr PC_CORE::RasterizerInfo rasterizerInfo =
      {
        .polygonMode = PC_CORE::PolygonMode::Fill,
        .cullModeFlag = PC_CORE::CullModeFlagBit::Back,
        .frontFace = PC_CORE::FrontFace::CounterClockwise
    };
    
    

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

    m_ShaderProgram = PC_CORE::ResourceManager::Create<PC_CORE::ShaderProgram>();*/
}

void PC_EDITOR_CORE::DebugDrawContext::DrawDebugPrimitive(PC_CORE::CommandList* _commandList,
                                                          const PC_CORE::RenderingContext& _renderingContext)
{
    if (_renderingContext.renderingContextFlag & PC_CORE::RenderingContextFlag::DebugDrawGeometry)
        return;


    
    
    
}
