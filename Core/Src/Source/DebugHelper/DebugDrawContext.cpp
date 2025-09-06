#include "DebugHelper/DebugDrawContext.hpp"


#include "Rendering/Renderer.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"

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

void PC_CORE::DebugDrawContext::DrawRay(Tbx::Vector3d _p1, Tbx::Vector3d _dir, float _distance, Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	const Tbx::Vector4f p1 = Tbx::Vector4f(static_cast<float>(_p1.x), static_cast<float>(_p1.y), static_cast<float>(_p1.z), 0);
	const Tbx::Vector4f dir = Tbx::Vector4f(static_cast<float>(_dir.x), static_cast<float>(_dir.y), static_cast<float>(_dir.z), 0);
	const Tbx::Vector4f color = Tbx::Vector4f(static_cast<float>(_color.x), static_cast<float>(_color.y), static_cast<float>(_color.z), 0);

	const Tbx::Vector4f p2 = p1 + (dir.Normalize() * _distance);


	m_Instance->m_RayPrimitiveData.rayBuffer.emplace_back(RayDataPerInstance
		{
		   p1,p2,color
		});
	m_Instance->m_RayPrimitiveData.rayCount++;
}

void PC_CORE::DebugDrawContext::DrawSphere(Tbx::Vector3d _p1, float _radius, Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	PushSphereGizmo(PrimitiveType::Sphere, _p1, _radius, _color);
}

void PC_CORE::DebugDrawContext::DrawBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size,
	Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	PushBoxGizmo(PrimitiveType::Box, _p1, euler, _size, _color);
}

void PC_CORE::DebugDrawContext::DrawWireBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size, Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	PushBoxGizmo(PrimitiveType::WireBox, _p1, euler, _size, _color);
}

void PC_CORE::DebugDrawContext::DrawCapsule(Tbx::Vector3d _p1, Tbx::Vector3d euler, float _radius, float _height, Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	PushCapsuleGizmo(PrimitiveType::Capsule, _p1, euler, _radius, _height, _color);
}

void PC_CORE::DebugDrawContext::DrawWireCapsule(Tbx::Vector3d _p1, Tbx::Vector3d euler, float _radius, float _height, Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	PushCapsuleGizmo(PrimitiveType::WireCapsule, _p1, euler, _radius, _height, _color);
}

void PC_CORE::DebugDrawContext::DrawWireSphere(Tbx::Vector3d _p1, float _radius, Tbx::Vector3f _color)
{
	if (m_Instance == nullptr)
		return;

	PushSphereGizmo(PrimitiveType::WireSphere, _p1, _radius, _color);
}

void PC_CORE::DebugDrawContext::Prepare()
{
	for (int i = 0; i < m_PrimitiveData.size(); i++)
	{
		size_t updateDataSize = m_PrimitiveData[i].matrixBuffer.size() * sizeof(Tbx::Matrix4x4f);
		assert(updateDataSize < GIZMO_BUFFER_SIZE && "updateDataSize should be less than GIZMO_BUFFER_SIZE");
		updateDataSize = std::clamp(updateDataSize, (size_t)0, GIZMO_BUFFER_SIZE);

		m_PrimitiveData[i].instanceBuffer.Update(m_PrimitiveData[i].matrixBuffer.data(),
			updateDataSize);

		m_PrimitiveData[i].primitiveCount = std::clamp(m_PrimitiveData[i].matrixBuffer.size(), (size_t)0, MAX_GIZMO_PRIMITIVE);
		m_PrimitiveData[i].matrixBuffer.clear();
	}

	{
		size_t updateRaySize = m_RayPrimitiveData.rayBuffer.size() * sizeof(RayDataPerInstance);

		assert(updateRaySize < RAY_BUFFER_SIZE && "updateRaySize should be less than RAY_BUFFER_SIZE");
		updateRaySize = std::clamp(updateRaySize, (size_t)0, RAY_BUFFER_SIZE);

		m_RayPrimitiveData.vertexBuffer.Update(m_RayPrimitiveData.rayBuffer.data(), updateRaySize);
		m_RayPrimitiveData.rayCount = std::clamp(m_RayPrimitiveData.rayBuffer.size(), (size_t)0, MAX_RAY_COUNT);

		m_RayPrimitiveData.rayBuffer.clear();
	}
}

void PC_CORE::DebugDrawContext::CreatePrimitiveShaders()
{
	constexpr PC_CORE::RasterizerInfo rasterizerInfo =
	{
		.polygonMode = PC_CORE::PolygonMode::Fill,
		.cullModeFlag = PC_CORE::CullModeFlagBit::None,
		.frontFace = PC_CORE::FrontFace::CounterClockwise
	};

	PC_CORE::VertexInputBindingDescrition primitiveInputBindingDescrition =
	{
		.binding = 0,
		.stride = sizeof(Tbx::Vector3f),
		.vertexInputRate = PC_CORE::VertexInputRate::VERTEX
	};

	PC_CORE::VertexInputBindingDescrition primitiveInstanceInputBindingDescrition =
	{
		.binding = 1,
		.stride = sizeof(Tbx::Matrix4x4f),
		.vertexInputRate = PC_CORE::VertexInputRate::INSTANCE
	};

	std::vector<PC_CORE::VertexAttributeDescription> attributeDescription;

	attributeDescription.push_back(
		{
		   .binding = 0,
		   .location = 0,
		   .format = RHIFormat::R32G32B32_SFLOAT,
		   .offset = 0
		});

	attributeDescription.push_back(
		{
			.binding = 1,
			.location = 1,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = 0
		});
	attributeDescription.push_back(
		{
			.binding = 1,
			.location = 2,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = sizeof(Tbx::Vector4f),
		});
	attributeDescription.push_back(
		{
			.binding = 1,
			.location = 3,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = sizeof(Tbx::Vector4f) * 2,
		});
	attributeDescription.push_back(
		{
			.binding = 1,
			.location = 4,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = sizeof(Tbx::Vector4f) * 3,
		});

	const PC_CORE::ShaderGraphicPointInfo shaderGraphicPointInfo =
	{
		.rasterizerInfo = rasterizerInfo,
		.dephInfo =
		{
			.depthCompareOp = CompareOp::LESS,
			.enableDepthTest = true
			},
		.vertexInputBindingDescritions =
		{
			primitiveInputBindingDescrition,
			primitiveInstanceInputBindingDescrition
		},
		.vertexAttributeDescriptions = attributeDescription,
	};


	const SourceList source =
	{
		{
			PC_CORE::ShaderStageType::Vertex,
			ResourceManager::Get<ShaderSourceBinary>("DebugDraw.vs.hlsl.binary")
		},
		{
			PC_CORE::ShaderStageType::Pixel,
			ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary")
		}
	};

	const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
	{
	.shaderGraphicPointInfo = shaderGraphicPointInfo,
	.sourceList = source,
	.renderPass = m_Renderer->renderPasses.forwardPass.get(),
		.colorAttachementCount = 1,
		.subPassIndex = 0,
	};


	m_ShaderProgram = PC_CORE::ResourceManager::Create<PC_CORE::GraphicShader>(
		"DebugGizmoShader", graphicShaderProgramCreateInfo);

	m_ShaderProgram.lock()->AllocDescriptorSet(&m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET);

	PC_CORE::UniformBufferDescriptor uniformBufferDescriptor
	{
		.buffer = &m_Renderer->uniformBuffers.cameraUniformBuffer
	};

	PC_CORE::ShaderProgramDescriptorWrite descriptor =
	{
		.shaderProgramDescriptorType = PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
		.bindingIndex = CAMERA_BINDING,
		.descriptor = uniformBufferDescriptor,
	};

	std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorWrites =
	{
		descriptor
	};
	m_ShaderProgramDescriptorSets->WriteDescriptorSets(descriptorWrites);
}

void PC_CORE::DebugDrawContext::CreateRayShaders()
{
	constexpr PC_CORE::RasterizerInfo rasterizerInfo =
	{
		.polygonMode = PC_CORE::PolygonMode::Line,
		.cullModeFlag = PC_CORE::CullModeFlagBit::None,
		.frontFace = PC_CORE::FrontFace::CounterClockwise
	};

	PC_CORE::VertexInputBindingDescrition vertexBindingDescrition =
	{
		.binding = 0,
		.stride = sizeof(RayDataPerInstance),
		.vertexInputRate = PC_CORE::VertexInputRate::INSTANCE
	};


	std::vector<PC_CORE::VertexAttributeDescription> attributeDescription;

	attributeDescription.push_back(
		{
			.binding = 0,
			.location = 0,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = 0
		});
	attributeDescription.push_back(
		{
			.binding = 0,
			.location = 1,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = sizeof(Tbx::Vector4f),
		});
	attributeDescription.push_back(
		{
			.binding = 0,
			.location = 2,
			.format = PC_CORE::RHIFormat::R32G32B32A32_SFLOAT,
			.offset = sizeof(Tbx::Vector4f) * 2,
		});

	const PC_CORE::ShaderGraphicPointInfo shaderGraphicPointInfo =
	{
		.rasterizerInfo = rasterizerInfo,
		.dephInfo =
	 {
			.depthCompareOp = CompareOp::LESS,
			.enableDepthTest = true
			},
		.vertexInputBindingDescritions = {vertexBindingDescrition},
		.vertexAttributeDescriptions = attributeDescription,
	};


	const SourceList source =
	{
		{
			PC_CORE::ShaderStageType::Vertex,
			ResourceManager::Get<ShaderSourceBinary>("DebugDrawRay.vs.hlsl.binary")

		},
		{
			PC_CORE::ShaderStageType::Pixel,
			ResourceManager::Get<ShaderSourceBinary>("DebugDraw.ps.hlsl.binary")

		}
	};

	const GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
	{
	.shaderGraphicPointInfo = shaderGraphicPointInfo,
	.sourceList = source,
	.renderPass = m_Renderer->renderPasses.forwardPass.get(),
	.colorAttachementCount = 1,
		.subPassIndex = 0,
	};

	m_ShaderProgramRay = PC_CORE::ResourceManager::Create<PC_CORE::GraphicShader>(
		"DebugGizmoShaderRay", graphicShaderProgramCreateInfo);
	m_ShaderProgramRay.lock()->AllocDescriptorSet(&m_ShaderProgramDescriptorSetsRay, SCENE_DESCRIPTOR_SET);

	PC_CORE::UniformBufferDescriptor uniformBufferDescriptor
	{
		.buffer = &m_Renderer->uniformBuffers.cameraUniformBuffer
	};

	PC_CORE::ShaderProgramDescriptorWrite descriptor =
	{
		.shaderProgramDescriptorType = PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
		.bindingIndex = CAMERA_BINDING,
		.descriptor = uniformBufferDescriptor,
	};

	std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorWrites =
	{
		descriptor
	};
	m_ShaderProgramDescriptorSetsRay->WriteDescriptorSets(descriptorWrites);
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

void PC_CORE::DebugDrawContext::DrawDebugPrimitive(PC_CORE::CommandList* _commandList,
	const PC_CORE::RenderingContext& _renderingContext)
{
	if (!(_renderingContext.renderingContextFlag & PC_CORE::RenderingContextFlag::DebugDrawGeometry))
		return;

	if (!NeedToRender())
		return;

	_commandList->BeginDebugLabel("Gizmo Pass", GIZMO_PASS);
	bool needReset = false;

	if (auto sh = m_ShaderProgram.lock())
	{
		_commandList->BindProgram(sh.get());

		for (size_t i = 0; i < m_PrimitiveData.size(); i++)
		{
			if (m_PrimitiveData[i].primitiveCount == 0)
				continue;

			if (i >= static_cast<size_t>((PrimitiveType::WireSphere)))
			{
				_commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyLineList);
				_commandList->SetLineWidth(1.f);
				needReset = true;
			}



			_commandList->BindDescriptorSet(sh.get(), m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET, 1);
			_commandList->BindVertexBuffer(*m_PrimitiveData[i].primitiveBuffer.GetRhiBuffer(), 0, 1);
			_commandList->BindVertexBuffer(*m_PrimitiveData[i].instanceBuffer.GetRhiBuffer(), 1, 1);

			_commandList->BindIndexBuffer(*m_PrimitiveData[i].primitiveIndexBuffer.GetRhiBuffer(), 0);

			_commandList->DrawIndexed(m_PrimitiveData[i].primitiveIndexBuffer.GetIndexCount(), m_PrimitiveData[i].primitiveCount, 0, 0, 0);
		}
	}

	// Ray
	if (m_RayPrimitiveData.rayCount != 0)
		if (auto sray = m_ShaderProgramRay.lock())
		{
			needReset = true;

			_commandList->BindProgram(sray.get());
			_commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyLineList);
			_commandList->SetLineWidth(1.f);
			_commandList->BindDescriptorSet(sray.get(), m_ShaderProgramDescriptorSets, SCENE_DESCRIPTOR_SET, 1);
			_commandList->BindVertexBuffer(*m_RayPrimitiveData.vertexBuffer.GetRhiBuffer(), 0, 1);
			_commandList->Draw(2, m_RayPrimitiveData.rayCount, 0, 0);
		}

	if (needReset)
		_commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleList);

	_commandList->EndDebugLabel();

}

PC_CORE::DebugDrawContext::DebugDrawContext(Renderer* _renderer)
	: m_Renderer(_renderer)
{
	m_Instance = this;

	CreatePrimitiveShaders();
	CreateRayShaders();

	for (size_t i = 0; i < (size_t)PrimitiveType::Count; i++)
	{
		GenerateBasePrimitve((PrimitiveType)i, &m_PrimitiveData[i].primitiveBuffer, &m_PrimitiveData[i].primitiveIndexBuffer);
	}


	for (size_t i = 0; i < m_PrimitiveData.size(); i++)
		m_PrimitiveData[i].instanceBuffer = PC_CORE::VertexBuffer(
			GIZMO_BUFFER_SIZE, PC_CORE::MemoryLocalisation::CPU_Only, PC_CORE::MemoryUsage::Dynamic);


	m_RayPrimitiveData.vertexBuffer = PC_CORE::VertexBuffer(
		RAY_BUFFER_SIZE, PC_CORE::MemoryLocalisation::CPU_Only, PC_CORE::MemoryUsage::Dynamic);
}



void PC_CORE::DebugDrawContext::PushBoxGizmo(PrimitiveType _primitiveType, Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size, Tbx::Vector3f _color)
{
	Tbx::Vector3f p1 = static_cast<Tbx::Vector3f>(_p1);

	Tbx::Matrix3x3f rotMatrix = Tbx::Rotation3x3<float>(static_cast<Tbx::Vector3f>(euler));

	const Tbx::Vector3f sz = static_cast<Tbx::Vector3f>(_size);
	const Tbx::Matrix3x3f matrix3 = rotMatrix * Tbx::Matrix3x3f
	(sz.x, 0.f, 0.f,
		0.f, sz.y, 0.f,
		0.f, 0.f, sz.z);

	const Tbx::Matrix4x4f m = Tbx::Matrix4x4f
	(matrix3.data[0], matrix3.data[1], matrix3.data[2], _color.x,
		matrix3.data[3], matrix3.data[4], matrix3.data[5], _color.y,
		matrix3.data[6], matrix3.data[7], matrix3.data[8], _color.z,
		p1.x, p1.y, p1.z, 1.f
	);

	m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(m);
}

void PC_CORE::DebugDrawContext::PushSphereGizmo(PrimitiveType _primitiveType, Tbx::Vector3d _p1, float _radius, Tbx::Vector3f _color)
{
	const Tbx::Vector3f p1 = static_cast<Tbx::Vector3f>(_p1);

	const Tbx::Matrix4x4f matrix = Tbx::Matrix4x4f
	(_radius, 0.f, 0.f, _color.x,
		0.f, _radius, 0.f, _color.y,
		0.f, 0.f, _radius, _color.z,
		p1.x, p1.y, p1.z, 1.f
	);
	m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(matrix);
}

void PC_CORE::DebugDrawContext::PushCapsuleGizmo(PrimitiveType _primitiveType, Tbx::Vector3d _p1, Tbx::Vector3d euler, float _radius, float _height, Tbx::Vector3f _color)
{
	Tbx::Vector3f p1 = static_cast<Tbx::Vector3f>(_p1);

	Tbx::Matrix3x3f rotMatrix = Tbx::Rotation3x3<float>(static_cast<Tbx::Vector3f>(euler));


	const Tbx::Matrix3x3f matrix3 = rotMatrix * Tbx::Matrix3x3f
	(_radius, 0.f, 0.f,
		0.f, _height, 0.f,
		0.f, 0.f, _radius);

	const Tbx::Matrix4x4f m = Tbx::Matrix4x4f
	(matrix3.data[0], matrix3.data[1], matrix3.data[2], _color.x,
		matrix3.data[3], matrix3.data[4], matrix3.data[5], _color.y,
		matrix3.data[6], matrix3.data[7], matrix3.data[8], _color.z,
		p1.x, p1.y, p1.z, 1.f
	);

	m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(m);
}


void PC_CORE::DebugDrawContext::GenerateBasePrimitve(PrimitiveType _primitiveType, VertexBuffer* _vertexBuffer,
	IndexBuffer* _indexBuffer)
{
	std::vector<Tbx::Vector3f> vertices;
	std::vector<uint32_t> indices;

	switch (_primitiveType)
	{
	case PC_CORE::DebugDrawContext::PrimitiveType::Sphere:
	{
		for (size_t lat = 0; lat <= 16; ++lat)
		{
			float theta = lat * M_PI / 16;  // Latitude angle (from 0 to PI)
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

			for (size_t lon = 0; lon <= 32; ++lon) {
				float phi = lon * 2 * M_PI / 32;  // Longitude angle (from 0 to 2PI)
				float sinPhi = sin(phi);
				float cosPhi = cos(phi);

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
			for (uint32_t lon = 0; lon < 32; ++lon) {
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
	case PC_CORE::DebugDrawContext::PrimitiveType::Box:
		vertices =
		{
		   Tbx::Vector3f{-0.5f, -0.5f, -0.5f}, // 0
		   Tbx::Vector3f{ 0.5f, -0.5f, -0.5f}, // 1
		   Tbx::Vector3f{ 0.5f,  0.5f, -0.5f}, // 2
		   Tbx::Vector3f{-0.5f,  0.5f, -0.5f}, // 3
		   Tbx::Vector3f{-0.5f, -0.5f,  0.5f}, // 4
		   Tbx::Vector3f{ 0.5f, -0.5f,  0.5f}, // 5
		   Tbx::Vector3f{ 0.5f,  0.5f,  0.5f}, // 6
			Tbx::Vector3f{-0.5f,  0.5f,  0.5f}  // 7
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
	case PC_CORE::DebugDrawContext::PrimitiveType::Capsule:
		GenerateCapsule(&vertices, &indices);
		break;
	case PC_CORE::DebugDrawContext::PrimitiveType::WireSphere:
	{
		constexpr size_t segments = 24;
		vertices.resize(segments * 6);

		for (size_t i = 0; i < segments; i++)
		{
			float theta0 = i * 2.0f * M_PI / segments;
			float theta1 = (i + 1) * 2.0f * M_PI / segments;
			const size_t offset = i * 6;

			// Circle XY
			vertices[offset + 0] = Tbx::Vector3f{ std::cos(theta0), std::sin(theta0), 0.0f };
			vertices[offset + 1] = Tbx::Vector3f{ std::cos(theta1), std::sin(theta1), 0.0f };

			// Circle XZ
			vertices[offset + 2] = Tbx::Vector3f{ std::cos(theta0), 0.0f, std::sin(theta0) };
			vertices[offset + 3] = Tbx::Vector3f{ std::cos(theta1), 0.0f, std::sin(theta1) };

			// Circle YZ
			vertices[offset + 4] = Tbx::Vector3f{ 0.0f, std::cos(theta0), std::sin(theta0) };
			vertices[offset + 5] = Tbx::Vector3f{ 0.0f, std::cos(theta1), std::sin(theta1) };
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
	case PC_CORE::DebugDrawContext::PrimitiveType::WireBox:
		vertices =
		{
		 Tbx::Vector3f{-0.5f, -0.5f, -0.5f}, // 0
		 Tbx::Vector3f{0.5f, -0.5f, -0.5f},  // 1
		 Tbx::Vector3f{0.5f, 0.5f, -0.5f},   // 2
		 Tbx::Vector3f{-0.5f, 0.5f, -0.5f},  // 3
		 Tbx::Vector3f{-0.5f, -0.5f, 0.5f},  // 4
		 Tbx::Vector3f{0.5f, -0.5f, 0.5f},   // 5
		 Tbx::Vector3f{0.5f, 0.5f, 0.5f},    // 6
		 Tbx::Vector3f{-0.5f, 0.5f, 0.5f}    // 7
		};
		indices =
		{
			 0, 1,   // Edge 1
			 1, 2,   // Edge 2
			 2, 3,   // Edge 3
			 3, 0,   // Edge 4
			 4, 5,   // Edge 5
			 5, 6,   // Edge 6
			 6, 7,   // Edge 7
			 7, 4,   // Edge 8
			 0, 4,   // Edge 9
			 1, 5,   // Edge 10
			 2, 6,   // Edge 11
			 3, 7    // Edge 12
		};
		break;
	case PC_CORE::DebugDrawContext::PrimitiveType::WireCapsule:
		GenerateWireCapsule(&vertices, &indices);
		break;
	case PC_CORE::DebugDrawContext::PrimitiveType::Count:
	default:
		assert(false);
		break;
	}

	if (!vertices.empty() && !indices.empty())
	{
		*_vertexBuffer = VertexBuffer(vertices.data(), vertices.size(), sizeof(Tbx::Vector3f), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static);
		*_indexBuffer = IndexBuffer(indices.data(), indices.size(), PC_CORE::MemoryLocalisation::GPU_Only, PC_CORE::MemoryUsage::Static);
	}
}
