#include "editor_sub_system/debug_draw_context.hpp"

#include "editor.hpp"
#include "resources/resource_manager.hpp"

#define GIZMO_PASS {1.f, 0.0f, 1.f, 0.5f}

void PC_EDITOR_CORE::DebugDrawContext::DrawSphere(Tbx::Vector3d _p1, float _radius, Tbx::Vector3f _color)
{
    Tbx::Vector3f p1 = _p1;

    const float segment = _radius * 2.f;
    const Tbx::Matrix4x4f matrix = Tbx::Matrix4x4f
    (       segment, 0.f, 0.f, _color.x,
            0.f, segment, 0.f, _color.y,
            0.f, 0.f, segment, _color.z,
            p1.x, p1.y, p1.z, 1.f
    );

    m_Instance->m_SphereGizmoBuffers.push_back(matrix);
}

void PC_EDITOR_CORE::DebugDrawContext::Render()
{
    EditorSubSystem::Render();

    vertexBufferSphere.Update(m_Instance->m_SphereGizmoBuffers.data(),
            m_Instance->m_SphereGizmoBuffers.size() * sizeof(Tbx::Matrix4x4f));
 
    
    m_SphereGizmoBufferCount = m_SphereGizmoBuffers.size();
    m_SphereGizmoBuffers.clear();
}

PC_EDITOR_CORE::DebugDrawContext::DebugDrawContext(Editor& _editor) : EditorSubSystem(_editor)
{
    m_Instance = this;
    m_SphereMesh = PC_CORE::ResourceManager::Get<PC_CORE::Mesh>("sphere.obj");
    
    m_Editor.gameApp.renderer.UserCustomForwardPass.push_back(
     std::bind(&PC_EDITOR_CORE::DebugDrawContext::DrawDebugPrimitive, this,
               std::placeholders::_1, std::placeholders::_2));
    
    CreateShaders();

    vertexBufferSphere = PC_CORE::VertexBuffer(sizeof(GpuBufferGizmo), PC_CORE::MemoryLocalisation::CPU_Only, PC_CORE::MemoryUsage::Dynamic);
}

void PC_EDITOR_CORE::DebugDrawContext::CreateShaders()
{
    
    constexpr PC_CORE::RasterizerInfo rasterizerInfo =
      {
        .polygonMode = PC_CORE::PolygonMode::Fill,
        .cullModeFlag = PC_CORE::CullModeFlagBit::Back,
        .frontFace = PC_CORE::FrontFace::CounterClockwise
    };
    
    PC_CORE::VertexInputBindingDescrition vertexBindingDescrition =
        {
        .binding = 1,
        .stride = sizeof(Tbx::Matrix4x4f),
        .vertexInputRate = PC_CORE::VertexInputRate::INSTANCE
        };

    auto attributeDescription = PC_CORE::Vertex::GetAttributeDescriptions(0);

    attributeDescription.push_back(
        {
            .binding = 1,
            .location = 3,
            .format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
            .offset = 0 
        });
        attributeDescription.push_back(
        {
            .binding = 1,
            .location = 4,
            .format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
            .offset = sizeof(Tbx::Vector4f),
        });
        attributeDescription.push_back(
       {
           .binding = 1,
           .location = 5,
           .format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
           .offset = sizeof(Tbx::Vector4f) * 2,
       });
    attributeDescription.push_back(
       {
           .binding = 1,
           .location = 6,
           .format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
           .offset = sizeof(Tbx::Vector4f) * 3,
       });
    
    const PC_CORE::ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo = rasterizerInfo,
        .vertexInputBindingDescritions = {PC_CORE::Vertex::GetBindingDescrition(0) , vertexBindingDescrition},
        .vertexAttributeDescriptions = attributeDescription,
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

    m_ShaderProgram = PC_CORE::ResourceManager::Create<PC_CORE::ShaderProgram>("DebugGizmoShader",_programShaderCreateInfo);
    m_ShaderProgram.lock()->AllocDescriptorSet(&m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET);

    PC_CORE::UniformBufferDescriptor uniformBufferDescriptor
    {
        .buffer = &m_Editor.gameApp.renderer.cameraUniformBuffer
    };
    
    PC_CORE::ShaderProgramDescriptorWrite descriptor =
        {
        .shaderProgramDescriptorType = PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
        .bindingIndex = CAMERA_BINDING,
        .uniformBufferDescriptor = &uniformBufferDescriptor,
        .imageSamperDescriptor = nullptr,
        };
    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorWrites =
        {
        descriptor
        };
    m_ShaderProgramDescriptorSets->WriteDescriptorSets(descriptorWrites);
}

void PC_EDITOR_CORE::DebugDrawContext::DrawDebugPrimitive(PC_CORE::CommandList* _commandList,
                                                          const PC_CORE::RenderingContext& _renderingContext)
{
    if (!(_renderingContext.renderingContextFlag & PC_CORE::RenderingContextFlag::DebugDrawGeometry))
        return;

    _commandList->BeginDebugLabel("Gizmo Pass", GIZMO_PASS);

    if (auto s = m_SphereMesh.lock(); auto sh = m_ShaderProgram.lock())
    {
        _commandList->BindProgram(sh.get());
        _commandList->BindDescriptorSet(sh.get(), m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET, 1);
        _commandList->BindVertexBuffer(*s->vertexBuffer.GetRhiBuffer(), 0, 1);
        _commandList->BindIndexBuffer(*s->indexBuffer.GetRhiBuffer(), 0);
        _commandList->BindVertexBuffer(*vertexBufferSphere.GetRhiBuffer(), 1, 1);
        
        _commandList->DrawIndexed(s->indexBuffer.GetIndexCount(), m_SphereGizmoBufferCount, 0 , 0, 0);
    }
    
}
