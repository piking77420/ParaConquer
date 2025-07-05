#include "debug_helper/debug_draw_context.hpp"


#include "rendering/renderer.hpp"
#include "resources/resource_manager.hpp"

#define GIZMO_PASS {1.f, 0.0f, 1.f, 0.5f}

void PC_CORE::DebugDrawContext::DrawSphere(Tbx::Vector3d _p1, float _radius, Tbx::Vector3f _color)
{
    Tbx::Vector3f p1 = static_cast<Tbx::Vector3f>(_p1);

    const float segment = _radius * 2.f;
    const Tbx::Matrix4x4f matrix = Tbx::Matrix4x4f
    (segment, 0.f, 0.f, _color.x,
     0.f, segment, 0.f, _color.y,
     0.f, 0.f, segment, _color.z,
     p1.x, p1.y, p1.z, 1.f
    );

    m_Instance->m_PrimitiveData[static_cast<size_t>(PrimitiveType::Sphere)].matrixBuffer.push_back(matrix);
}

void PC_CORE::DebugDrawContext::DrawBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size,
    Tbx::Vector3f _color)
{
    Tbx::Vector3f p1 = static_cast<Tbx::Vector3f>(_p1);

    Tbx::Matrix3x3f rotMatrix = Tbx::Rotation3x3<float>(static_cast<Tbx::Vector3f>(euler));

    const Tbx::Vector3f sz = static_cast<Tbx::Vector3f>(_size);
    const Tbx::Matrix3x3f matrix3 = rotMatrix * Tbx::Matrix3x3f
    (sz.x, 0.f, 0.f,
     0.f, sz.y, 0.f,
     0.f, 0.f, sz.z);

    const Tbx::Matrix4x4f m = Tbx::Matrix4x4f
    (   matrix3.data[0], matrix3.data[1], matrix3.data[2], _color.x,
        matrix3.data[3], matrix3.data[4], matrix3.data[5], _color.y,
        matrix3.data[6], matrix3.data[7], matrix3.data[8], _color.z,
        p1.x, p1.y, p1.z, 1.f
    );

    m_Instance->m_PrimitiveData[static_cast<size_t>(PrimitiveType::Box)].matrixBuffer.push_back(m);
}

void PC_CORE::DebugDrawContext::DrawWireSphere(Tbx::Vector3d _p1, float _radius, Tbx::Vector3f _color)
{
    Tbx::Vector3f p1 = static_cast<Tbx::Vector3f>(_p1);

    const float segment = _radius * 2.f;
    const Tbx::Matrix4x4f matrix = Tbx::Matrix4x4f
    (segment, 0.f, 0.f, _color.x,
     0.f, segment, 0.f, _color.y,
     0.f, 0.f, segment, _color.z,
     p1.x, p1.y, p1.z, 1.f
    );
    m_Instance->m_PrimitiveData[static_cast<size_t>(PrimitiveType::WireSphere)].matrixBuffer.push_back(matrix);

}

void PC_CORE::DebugDrawContext::Prepare()
{
    for (int i = 0; i < m_PrimitiveData.size(); i++)
    {
        m_PrimitiveData[i].primitiveVertexBuffer.Update(m_PrimitiveData[i].matrixBuffer.data(),
                                                        m_PrimitiveData[i].matrixBuffer.size() * sizeof(
                                                            Tbx::Matrix4x4f));

        m_PrimitiveData[i].primitiveCount = m_PrimitiveData[i].matrixBuffer.size();
        m_PrimitiveData[i].matrixBuffer.clear();
    }
}

void PC_CORE::DebugDrawContext::CreateShaders()
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
        .vertexInputBindingDescritions = {PC_CORE::Vertex::GetBindingDescrition(0), vertexBindingDescrition},
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
    };


    PC_CORE::ProgramShaderCreateInfo _programShaderCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = m_Renderer->forwardPass,
    };

    m_ShaderProgram = PC_CORE::ResourceManager::Create<PC_CORE::ShaderProgram>(
        "DebugGizmoShader", _programShaderCreateInfo);
    m_ShaderProgram.lock()->AllocDescriptorSet(&m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET);

    PC_CORE::UniformBufferDescriptor uniformBufferDescriptor
    {
        .buffer = &m_Renderer->cameraUniformBuffer
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

void PC_CORE::DebugDrawContext::DrawDebugPrimitive(PC_CORE::CommandList* _commandList,
                                                   const PC_CORE::RenderingContext& _renderingContext)
{
    if (!(_renderingContext.renderingContextFlag & PC_CORE::RenderingContextFlag::DebugDrawGeometry))
        return;

    _commandList->BeginDebugLabel("Gizmo Pass", GIZMO_PASS);
    bool needReset = false;

    if  (auto sh = m_ShaderProgram.lock())
    {
        _commandList->BindProgram(sh.get());

        for (size_t i = 0; i < m_PrimitiveData.size(); i++)
        {
            if (m_PrimitiveData[i].primitiveCount == 0)
                continue;
            
            if (i >= static_cast<size_t>((PrimitiveType::WireSphere)))
            {
                _commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyLineStrip);
                _commandList->SetLineWidth(1.f);
                needReset = true;
            }

            if (auto s = m_PrimitiveData[i].mesh.lock())
            {
                _commandList->BindDescriptorSet(sh.get(), m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET, 1);
                _commandList->BindVertexBuffer(*s->vertexBuffer.GetRhiBuffer(), 0, 1);
                _commandList->BindIndexBuffer(*s->indexBuffer.GetRhiBuffer(), 0);
                _commandList->BindVertexBuffer(*m_PrimitiveData[i].primitiveVertexBuffer.GetRhiBuffer(), 1, 1);

                _commandList->DrawIndexed(s->indexBuffer.GetIndexCount(), m_PrimitiveData[i].primitiveCount, 0, 0, 0);
            }
        }
    }
    
    // reset
    if (needReset)
        _commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleList);
    
    _commandList->EndDebugLabel();
  
}

PC_CORE::DebugDrawContext::DebugDrawContext(Renderer* _renderer)
    : m_Renderer(_renderer)
{
    m_Instance = this;

    CreateShaders();

    // Init primitive
    size_t primitiveIndex = static_cast<size_t>(PrimitiveType::Sphere);
    m_PrimitiveData[primitiveIndex].mesh = ResourceManager::Get<Mesh>("sphere.obj");

    primitiveIndex = static_cast<size_t>(PrimitiveType::Box);
    m_PrimitiveData[primitiveIndex].mesh = ResourceManager::Get<Mesh>("cube.obj");

    primitiveIndex = static_cast<size_t>(PrimitiveType::Capusle);
    m_PrimitiveData[primitiveIndex].mesh = ResourceManager::Get<Mesh>("capsule.obj");

    primitiveIndex = static_cast<size_t>(PrimitiveType::WireSphere);
    m_PrimitiveData[primitiveIndex].mesh = ResourceManager::Get<Mesh>("sphere.obj");

    primitiveIndex = static_cast<size_t>(PrimitiveType::WireBox);
    m_PrimitiveData[primitiveIndex].mesh = ResourceManager::Get<Mesh>("cube.obj");

    primitiveIndex = static_cast<size_t>(PrimitiveType::WireCapsule);
    m_PrimitiveData[primitiveIndex].mesh = ResourceManager::Get<Mesh>("capsule.obj");


    for (size_t i = 0; i < m_PrimitiveData.size(); i++)
        m_PrimitiveData[i].primitiveVertexBuffer = PC_CORE::VertexBuffer(
            sizeof(GpuBufferGizmo), PC_CORE::MemoryLocalisation::CPU_Only, PC_CORE::MemoryUsage::Dynamic);
}
