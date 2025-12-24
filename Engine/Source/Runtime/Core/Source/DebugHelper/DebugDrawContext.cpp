#include "DebugHelper/DebugDrawContext.hpp"


#include "Rendering/Renderer.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"
#include "LowRenderer/Rhi.hpp"

#define GIZMO_PASS {1.f, 0.0f, 1.f, 0.5f}

#pragma region GeneratePrimitive
// Thanks to Chat GPT hehe
void GenerateCapsule(std::vector<Tbx::Vector3f>* _verticies, std::vector<uint32_t>* _indicies)
{
}

void GenerateWireCapsule(std::vector<Tbx::Vector3f>* _vertices, std::vector<uint32_t>* _indices)
{
}

#pragma endregion GeneratePrimitive

void PC_CORE::DebugDrawContext::DrawRay(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _dir, const float _distance, const Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    const auto p1 = Tbx::Vector4f(static_cast<float>(_p1.x), static_cast<float>(_p1.y), static_cast<float>(_p1.z), 0);
    const auto dir = Tbx::Vector4f(static_cast<float>(_dir.x), static_cast<float>(_dir.y), static_cast<float>(_dir.z),
                                   0);
    const auto color = Tbx::Vector4f(_color.x, _color.y, _color.z, 0);

    const Tbx::Vector4f p2 = p1 + (dir.Normalize() * _distance);


    m_Instance->m_RayPrimitiveData.rayBuffer.emplace_back(RayDataPerInstance
        {
            p1, p2, color
        });
    m_Instance->m_RayPrimitiveData.rayCount++;
}

void PC_CORE::DebugDrawContext::DrawSphere(const Tbx::Vector3d& _p1, const float _radius, const Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    PushSphereGizmo(PrimitiveType::Sphere, _p1, _radius, _color);
}

void PC_CORE::DebugDrawContext::DrawBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
                                        Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    PushBoxGizmo(PrimitiveType::Box, _p1, euler, _size, _color);
}

void PC_CORE::DebugDrawContext::DrawWireSphere(const Tbx::Vector3d& _p1, float _radius, Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    PushSphereGizmo(PrimitiveType::WireSphere, _p1, _radius, _color);
}

void PC_CORE::DebugDrawContext::DrawWireBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, const Tbx::Vector3d& _size,
                                            const Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    PushBoxGizmo(PrimitiveType::WireBox, _p1, _euler, _size, _color);
}

void PC_CORE::DebugDrawContext::DrawCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, float _radius, float _height,
                                            Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    PushCapsuleGizmo(PrimitiveType::Capsule, _p1, _euler, _radius, _height, _color);
}

void PC_CORE::DebugDrawContext::DrawWireCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, float _radius, float _height,
                                                Tbx::Vector3f _color)
{
    if (m_Instance == nullptr)
        return;

    PushCapsuleGizmo(PrimitiveType::WireCapsule, _p1, euler, _radius, _height, _color);
}

void PC_CORE::DebugDrawContext::DrawDebugPrimitive(CommandList* _commandList,
                                                   const RenderingContext& _renderingContext)
{
    if (!(_renderingContext.RenderingContextFlag & DebugDrawGeometry))
        return;

    if (!NeedToRender())
        return;

    _commandList->BeginDebugLabel("Gizmo Pass", GIZMO_PASS);
    bool needReset = false;

   
    _commandList->BindProgram(*m_ShaderProgram.get());

    for (size_t i = 0; i < m_PrimitiveData.size(); i++)
    {
        if (m_PrimitiveData[i].primitiveCount == 0)
            continue;

        if (i >= static_cast<size_t>((PrimitiveType::WireSphere)))
        {
            _commandList->SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyLineList);
            _commandList->SetLineWidth(1.f);
            needReset = true;
        }


        _commandList->BindDescriptorSet(*m_ShaderProgram.get(), m_ShaderProgramDescriptorSets.get(), SCENE_DESCRIPTOR_SET, 1);
        _commandList->BindVertexBuffer(*m_PrimitiveData[i].primitiveBuffer.Get(), 0, 1);
        _commandList->BindVertexBuffer(*m_PrimitiveData[i].instanceBuffer, 1, 1);

        _commandList->BindIndexBuffer(*m_PrimitiveData[i].primitiveIndexBuffer.Get(), m_PrimitiveData[i].primitiveIndexBuffer.GetIndexFormat(), 0);

        _commandList->DrawIndexed(m_PrimitiveData[i].primitiveIndexBuffer.GetIndexCount(),
                                    m_PrimitiveData[i].primitiveCount, 0, 0, 0);
    }
    

    // Ray
    if (m_RayPrimitiveData.rayCount != 0)
    {
        needReset = true;
        _commandList->BindProgram(*m_ShaderProgramRay.get());
        _commandList->SetPrimitiveTopology(RhiShader::PrimitiveTopologyLineList);
        _commandList->SetLineWidth(1.f);
        _commandList->BindDescriptorSet(*m_ShaderProgramRay.get(), m_ShaderProgramDescriptorSets.get(), SCENE_DESCRIPTOR_SET, 1);
        _commandList->BindVertexBuffer(*m_RayPrimitiveData.vertexBuffer.Get(), 0, 1);
        _commandList->Draw(2, static_cast<uint32_t>(m_RayPrimitiveData.rayCount), 0, 0);
    }
       
        

    if (needReset)
        _commandList->SetPrimitiveTopology(RhiShader::PrimitiveTopologyTriangleList);

    _commandList->EndDebugLabel();
}

void PC_CORE::DebugDrawContext::Prepare()
{
    for (int i = 0; i < m_PrimitiveData.size(); i++)
    {
        
        size_t updateDataSize = m_PrimitiveData[i].matrixBuffer.size() * sizeof(Tbx::Matrix4x4f);
        assert(updateDataSize < GIZMO_BUFFER_SIZE && "updateDataSize should be less than GIZMO_BUFFER_SIZE");
        updateDataSize = std::clamp(updateDataSize, static_cast<size_t>(0), GIZMO_BUFFER_SIZE);

        if (char* ptr = m_PrimitiveData[i].instanceBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
        {
            std::memcpy(ptr, m_PrimitiveData[i].matrixBuffer.data(), updateDataSize);
            m_PrimitiveData[i].instanceBuffer->EndFullDynamicBufferUpdateForCurrentFrame();
        }
        m_PrimitiveData[i].primitiveCount = std::clamp(m_PrimitiveData[i].matrixBuffer.size(), static_cast<size_t>(0),
                                                       MAX_GIZMO_PRIMITIVE);
        m_PrimitiveData[i].matrixBuffer.clear();
    }

    {
        
        size_t updateRaySize = m_RayPrimitiveData.rayBuffer.size() * sizeof(RayDataPerInstance);

        assert(updateRaySize < RAY_BUFFER_SIZE && "updateRaySize should be less than RAY_BUFFER_SIZE");
        updateRaySize = std::clamp(updateRaySize, static_cast<size_t>(0), RAY_BUFFER_SIZE);
        if (char* ptr = m_RayPrimitiveData.vertexBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
        {
            std::memcpy(ptr, m_RayPrimitiveData.rayBuffer.data(), updateRaySize);
            m_RayPrimitiveData.vertexBuffer->EndFullDynamicBufferUpdateForCurrentFrame();
        }
        m_RayPrimitiveData.rayCount = std::clamp(m_RayPrimitiveData.rayBuffer.size(), static_cast<size_t>(0),
                                                 MAX_RAY_COUNT);
        m_RayPrimitiveData.rayBuffer.clear();
    }
}

PC_CORE::DebugDrawContext::DebugDrawContext(Renderer* _renderer)
    : m_Renderer(_renderer)
{
    m_Instance = this;

    CreatePrimitiveShaders();
    CreateRayShaders();

    for (size_t i = 0; i < static_cast<size_t>(PrimitiveType::Count); i++)
    {
        GenerateBasePrimitve(static_cast<PrimitiveType>(i), &m_PrimitiveData[i].primitiveBuffer,
                             &m_PrimitiveData[i].primitiveIndexBuffer);
    }


    int i = 0;
    for (auto& primitiveData : m_PrimitiveData)
    {
        primitiveData.instanceBuffer.reset(_renderer->GetRhi().CreateBuffer());
        primitiveData.instanceBuffer
            ->SetSize(GIZMO_BUFFER_SIZE)
            .SetMemoryUsage(RhiMemoryUsage::Dynamic)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex)
            .SetName("Buffer " + PrimitiveTypeToString(static_cast<PrimitiveType>(i)))
            .Build();

        i++;
    }
    m_RayPrimitiveData.vertexBuffer
        ->SetSize(RAY_BUFFER_SIZE)
        .SetMemoryUsage(RhiMemoryUsage::Dynamic)
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex)
        .SetName("Buffer Ray Gizmo")
        .Build();
}

void PC_CORE::DebugDrawContext::CreatePrimitiveShaders()
{
    VertexInputBindingDescrition primitiveInputBindingDescrition =
    {
        .Binding = 0,
        .Stride = sizeof(Tbx::Vector3f),
        .VertexInputRate = VertexInputRate::Vertex
    };

    VertexInputBindingDescrition primitiveInstanceInputBindingDescrition =
    {
        .Binding = 1,
        .Stride = sizeof(Tbx::Matrix4x4f),
        .VertexInputRate = VertexInputRate::Instance
    };

    std::vector<VertexAttributeDescription> attributeDescription;

    attributeDescription.push_back(
        {
            .Binding = 0,
            .Location = 0,
            .Format = RhiFormat::R32G32B32Sfloat,
            .Offset = 0
        });

    attributeDescription.push_back(
        {
            .Binding = 1,
            .Location = 1,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = 0
        });
    attributeDescription.push_back(
        {
            .Binding = 1,
            .Location = 2,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = sizeof(Tbx::Vector4f),
        });
    attributeDescription.push_back(
        {
            .Binding = 1,
            .Location = 3,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = sizeof(Tbx::Vector4f) * 2,
        });
    attributeDescription.push_back(
        {
            .Binding = 1,
            .Location = 4,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = sizeof(Tbx::Vector4f) * 3,
        });

    std::vector<RhiShaderProgram::ShaderModule> shaderModule =
    {
        {
            RhiShaderProgram::ShaderStageType::Vertex,
            ResourceManager::Get<ShaderSourceBinary>("DebugDraw.vs.hlsl.binary")->GetCode()
        },
        {
            RhiShaderProgram::ShaderStageType::Pixel,
            ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary")->GetCode()
        }
    };

    m_ShaderProgram.reset(m_Renderer->GetRhi().CreateRhiShaderProgram());
    m_ShaderProgram->SetShaderModules(shaderModule)
        .SetPipelineType(RhiShader::PipelineType::Graphic)
        .SetCullMode(RhiShader::CullNone)
        .SetFrontFace(RhiShader::FrontFace::CounterClockwise)
        .SetDepthTest(true)
        .SetVertexInputBindingDescritions({ primitiveInputBindingDescrition })
        .SetVertexAttributeDescriptions(attributeDescription)
        .SetRenderPass(*m_Renderer->RenderPasses.ForwardPass)
        .SetAttachementCount(1)
        .SetSubPassIndex(0)
        .SetName("DebugGizmoShader")
        .Build();

    // Binding
    m_ShaderProgramDescriptorSets.reset(m_ShaderProgram->CreateDescriptorBinding());

    BufferDescriptor uniformBufferDescriptor
    {
        .buffer = m_Renderer->UniformBuffers.Camera.get()
    };

    DescriptorWrite descriptor =
    {
        .type = DescriptorType::UniformBuffer,
        .bindingIndex = CAMERA_BINDING,
        .descriptor = uniformBufferDescriptor,
    };

  
    m_ShaderProgramDescriptorSets
        ->SetBindings(SCENE_DESCRIPTOR_SET, DescriptorWrite{ DescriptorType::UniformBuffer, CAMERA_BINDING, uniformBufferDescriptor })
        .SetName("DebugGizmoShader Bindings")
        .Build();
}

void PC_CORE::DebugDrawContext::CreateRayShaders()
{
    
    VertexInputBindingDescrition vertexBindingDescrition =
    {
        .Binding = 0,
        .Stride = sizeof(RayDataPerInstance),
        .VertexInputRate = VertexInputRate::Instance
    };


    std::vector<VertexAttributeDescription> attributeDescription;

    attributeDescription.push_back(
        {
            .Binding = 0,
            .Location = 0,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = 0
        });
    attributeDescription.push_back(
        {
            .Binding = 0,
            .Location = 1,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = sizeof(Tbx::Vector4f),
        });
    attributeDescription.push_back(
        {
            .Binding = 0,
            .Location = 2,
            .Format = RhiFormat::R32G32B32A32Sfloat,
            .Offset = sizeof(Tbx::Vector4f) * 2,
        });

    const std::vector<RhiShader::ShaderModule> shaderModule =
    {
        {
            RhiShaderProgram::ShaderStageType::Vertex,
            ResourceManager::Get<ShaderSourceBinary>("DebugDrawRay.vs.hlsl.binary")->GetCode()

        },
        {
            RhiShaderProgram::ShaderStageType::Pixel,
            ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary")->GetCode()

        }
    };

    m_ShaderProgramRay.reset(m_Renderer->GetRhi().CreateRhiShaderProgram());
    m_ShaderProgramRay->SetShaderModules(shaderModule)
        .SetPipelineType(RhiShader::PipelineType::Graphic)
        .SetPolygonMode(RhiShader::PolygonMode::Line)
        .SetCullMode(RhiShader::CullNone)
        .SetFrontFace(RhiShader::FrontFace::CounterClockwise)
        .SetDepthTest(true)
        .SetVertexInputBindingDescritions({ vertexBindingDescrition })
        .SetVertexAttributeDescriptions(attributeDescription)
        .SetRenderPass(*m_Renderer->RenderPasses.ForwardPass)
        .SetAttachementCount(1)
        .SetSubPassIndex(0)
        .SetName("DebugGizmoShaderRay")
        .Build();

    // Binding
    m_ShaderProgramDescriptorSetsRay.reset(m_ShaderProgramRay->CreateDescriptorBinding());
    
    const DescriptorWrite descriptor =
    {
        .type = DescriptorType::UniformBuffer,
        .bindingIndex = CAMERA_BINDING,
        .descriptor = BufferDescriptor(m_Renderer->UniformBuffers.Camera.get()),
    };

    m_ShaderProgramDescriptorSetsRay
        ->SetBindings(SCENE_DESCRIPTOR_SET, descriptor)
        .SetName("DebugDrawGizmo Ray Binding ")
        .Build();
}

bool PC_CORE::DebugDrawContext::NeedToRender()
{
    for (size_t i = 0; i < m_PrimitiveData.size(); i++)
    {
        if (m_PrimitiveData[i].primitiveCount > 0)
            return true;
    }

    if (m_RayPrimitiveData.rayCount > 0)
        return true;

    return false;
}


void PC_CORE::DebugDrawContext::GenerateBasePrimitve(PrimitiveType _primitiveType, VertexBuffer* _vertexBuffer,
                                                     IndexBuffer* _indexBuffer)
{
    std::vector<Tbx::Vector3f> vertices;
    std::vector<uint32_t> indices;

    switch (_primitiveType)
    {
    case PrimitiveType::Sphere:
        {
            for (size_t lat = 0; lat <= 16; ++lat)
            {
                float theta = lat * M_PI / 16; // Latitude angle (from 0 to PI)
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);

                for (size_t lon = 0; lon <= 32; ++lon)
                {
                    float phi = lon * 2 * M_PI / 32; // Longitude angle (from 0 to 2PI)
                    float sinPhi = std::sin(phi);
                    float cosPhi = std::cos(phi);

                    Tbx::Vector3f vertex;
                    vertex.x = cosPhi * sinTheta;
                    vertex.y = cosTheta;
                    vertex.z = sinPhi * sinTheta;
                    vertices.push_back(vertex);
                }
            }

            // Define indices
            for (uint32_t lat = 0; lat < 16; ++lat)
            {
                for (uint32_t lon = 0; lon < 32; ++lon)
                {
                    uint32_t first = lat * (32 + 1) + lon;
                    uint32_t second = first + 32 + 1;

                    // Triangle 1: first, second, first+1
                    indices.push_back(first);
                    indices.push_back(second);
                    indices.push_back(first + 1);

                    // Triangle 2: second, second+1, first+1
                    indices.push_back(second);
                    indices.push_back(second + 1);
                    indices.push_back(first + 1);
                }
            }
        }

        break;
    case PrimitiveType::Box:
        vertices =
        {
            Tbx::Vector3f{-0.5f, -0.5f, -0.5f}, // 0
            Tbx::Vector3f{0.5f, -0.5f, -0.5f}, // 1
            Tbx::Vector3f{0.5f, 0.5f, -0.5f}, // 2
            Tbx::Vector3f{-0.5f, 0.5f, -0.5f}, // 3
            Tbx::Vector3f{-0.5f, -0.5f, 0.5f}, // 4
            Tbx::Vector3f{0.5f, -0.5f, 0.5f}, // 5
            Tbx::Vector3f{0.5f, 0.5f, 0.5f}, // 6
            Tbx::Vector3f{-0.5f, 0.5f, 0.5f} // 7
        };

        indices =
        {
            // Bottom face
            0, 1, 2,
            0, 2, 3,
            // Top face
            4, 5, 6,
            4, 6, 7,
            // Front face
            0, 1, 5,
            0, 5, 4,
            // Back face
            2, 3, 7,
            2, 7, 6,
            // Left face
            0, 4, 7,
            0, 7, 3,
            // Right face
            1, 2, 6,
            1, 6, 5
        };

        break;
    case PrimitiveType::Capsule:
        GenerateCapsule(&vertices, &indices);
        break;
    case PrimitiveType::WireSphere:
        {
            constexpr size_t segments = 24;
            vertices.resize(segments * 6);

            for (size_t i = 0; i < segments; i++)
            {
                float theta0 = i * 2.0f * M_PI / segments;
                float theta1 = (i + 1) * 2.0f * M_PI / segments;
                const size_t offset = i * 6;

                // Circle XY
                vertices[offset + 0] = Tbx::Vector3f{std::cos(theta0), std::sin(theta0), 0.0f};
                vertices[offset + 1] = Tbx::Vector3f{std::cos(theta1), std::sin(theta1), 0.0f};

                // Circle XZ
                vertices[offset + 2] = Tbx::Vector3f{std::cos(theta0), 0.0f, std::sin(theta0)};
                vertices[offset + 3] = Tbx::Vector3f{std::cos(theta1), 0.0f, std::sin(theta1)};

                // Circle YZ
                vertices[offset + 4] = Tbx::Vector3f{0.0f, std::cos(theta0), std::sin(theta0)};
                vertices[offset + 5] = Tbx::Vector3f{0.0f, std::cos(theta1), std::sin(theta1)};
            }

            // Generate indices for LineList
            for (size_t i = 0; i < segments; ++i)
            {
                size_t baseIndex = i * 6;

                // For XY Circle
                indices.push_back(baseIndex + 0);
                indices.push_back(baseIndex + 1);

                // For XZ Circle
                indices.push_back(baseIndex + 2);
                indices.push_back(baseIndex + 3);

                // For YZ Circle
                indices.push_back(baseIndex + 4);
                indices.push_back(baseIndex + 5);
            }

            // To connect the last segment to the first segment to close the circles
            for (int circleOffset = 0; circleOffset < 6; circleOffset += 2)
            {
                size_t lastIndex = (segments - 1) * 6 + circleOffset;
                size_t firstIndex = circleOffset;
                indices.push_back(lastIndex);
                indices.push_back(firstIndex);
            }
        }
        break;
    case PrimitiveType::WireBox:
        vertices =
        {
            Tbx::Vector3f{-0.5f, -0.5f, -0.5f}, // 0
            Tbx::Vector3f{0.5f, -0.5f, -0.5f}, // 1
            Tbx::Vector3f{0.5f, 0.5f, -0.5f}, // 2
            Tbx::Vector3f{-0.5f, 0.5f, -0.5f}, // 3
            Tbx::Vector3f{-0.5f, -0.5f, 0.5f}, // 4
            Tbx::Vector3f{0.5f, -0.5f, 0.5f}, // 5
            Tbx::Vector3f{0.5f, 0.5f, 0.5f}, // 6
            Tbx::Vector3f{-0.5f, 0.5f, 0.5f} // 7
        };
        indices =
        {
            0, 1, // Edge 1
            1, 2, // Edge 2
            2, 3, // Edge 3
            3, 0, // Edge 4
            4, 5, // Edge 5
            5, 6, // Edge 6
            6, 7, // Edge 7
            7, 4, // Edge 8
            0, 4, // Edge 9
            1, 5, // Edge 10
            2, 6, // Edge 11
            3, 7 // Edge 12
        };
        break;
    case PrimitiveType::WireCapsule:
        GenerateWireCapsule(&vertices, &indices);
        break;
    case PrimitiveType::Count:
    default:
        assert(false);
        break;
    }

    if (!vertices.empty() && !indices.empty())
    {
        VertexBuffer& VertexBuffer = *_vertexBuffer;
        IndexBuffer& IndexBuffer = *_indexBuffer;

        // TODO UPOLOAD DATA
        /*
        *_vertexBuffer = VertexBuffer(vertices.data(), vertices.size(), sizeof(Tbx::Vector3f),
                                      MemoryLocalisation::GpuOnly, MemoryUsage::Static);


        *_indexBuffer = IndexBuffer(indices.data(), indices.size(), MemoryLocalisation::GpuOnly, MemoryUsage::Static);*/
    }
}

void PC_CORE::DebugDrawContext::PushBoxGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
                                             const Tbx::Vector3d& _size, Tbx::Vector3f _color)
{
    auto p1 = static_cast<Tbx::Vector3f>(_p1);

    Tbx::Matrix3x3f rotMatrix = Tbx::Rotation3x3<float>(static_cast<Tbx::Vector3f>(euler));

    const auto sz = static_cast<Tbx::Vector3f>(_size);
    const Tbx::Matrix3x3f matrix3 = rotMatrix * Tbx::Matrix3x3f
    (sz.x, 0.f, 0.f,
     0.f, sz.y, 0.f,
     0.f, 0.f, sz.z);

    const auto m = Tbx::Matrix4x4f
    (matrix3.data[0], matrix3.data[1], matrix3.data[2], _color.x,
     matrix3.data[3], matrix3.data[4], matrix3.data[5], _color.y,
     matrix3.data[6], matrix3.data[7], matrix3.data[8], _color.z,
     p1.x, p1.y, p1.z, 1.f
    );

    m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(m);
}

void PC_CORE::DebugDrawContext::PushSphereGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, float _radius,
                                                Tbx::Vector3f _color)
{
    const auto p1 = static_cast<Tbx::Vector3f>(_p1);

    const auto matrix = Tbx::Matrix4x4f
    (_radius, 0.f, 0.f, _color.x,
     0.f, _radius, 0.f, _color.y,
     0.f, 0.f, _radius, _color.z,
     p1.x, p1.y, p1.z, 1.f
    );
    m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(matrix);
}


void PC_CORE::DebugDrawContext::PushCapsuleGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
                                                 float _radius, float _height, Tbx::Vector3f _color)
{
    auto p1 = static_cast<Tbx::Vector3f>(_p1);

    Tbx::Matrix3x3f rotMatrix = Tbx::Rotation3x3<float>(static_cast<Tbx::Vector3f>(euler));


    const Tbx::Matrix3x3f matrix3 = rotMatrix * Tbx::Matrix3x3f
    (_radius, 0.f, 0.f,
     0.f, _height, 0.f,
     0.f, 0.f, _radius);

    const auto m = Tbx::Matrix4x4f
    (matrix3.data[0], matrix3.data[1], matrix3.data[2], _color.x,
     matrix3.data[3], matrix3.data[4], matrix3.data[5], _color.y,
     matrix3.data[6], matrix3.data[7], matrix3.data[8], _color.z,
     p1.x, p1.y, p1.z, 1.f
    );

    m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(m);
}

std::string PC_CORE::DebugDrawContext::PrimitiveTypeToString(PrimitiveType _primitiveType)
{
    switch (_primitiveType)
    {
    case PrimitiveType::Sphere:
        return "Sphere";
    case PrimitiveType::Box:
        return "Box";
    case PrimitiveType::Capsule:
        return "Capsule";
    case PrimitiveType::WireSphere:
        return "WireSphere";
    case PrimitiveType::WireBox:
        return "WireBox";
    case PrimitiveType::WireCapsule:
        return "WireCapsule";
    case PrimitiveType::Count:
        break;
    }
    return "";
}
