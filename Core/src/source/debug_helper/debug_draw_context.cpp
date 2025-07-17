#include "debug_helper/debug_draw_context.hpp"


#include "rendering/renderer.hpp"
#include "resources/resource_manager.hpp"
#include "resources/shader_source_binary.hpp"

#define GIZMO_PASS {1.f, 0.0f, 1.f, 0.5f}

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

	PushCapsuleGizmo(PrimitiveType::Capusle, _p1, euler, _radius, _height, _color);
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

		m_PrimitiveData[i].primitiveVertexBuffer.Update(m_PrimitiveData[i].matrixBuffer.data(),
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
		.dephInfo =
		{
			.depthCompareOp = CompareOp::LESS,
			.enableDepthTest = true
			},
		.vertexInputBindingDescritions = {PC_CORE::Vertex::GetBindingDescrition(0), vertexBindingDescrition},
		.vertexAttributeDescriptions = attributeDescription,
	};


	const SourceList source =
	{
		{
			PC_CORE::ShaderStageTypeFlag::Vertex,
			ResourceManager::Get<ShaderSourceBinary>("debug_draw_spv.vert")
		},
		{
			PC_CORE::ShaderStageTypeFlag::Fragment,
			ResourceManager::Get<ShaderSourceBinary>("debug_draw_spv.frag")
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
			PC_CORE::ShaderStageTypeFlag::Vertex,
			ResourceManager::Get<ShaderSourceBinary>("debug_draw_ray_spv.vert")

		},
		{
			PC_CORE::ShaderStageTypeFlag::Fragment,
			ResourceManager::Get<ShaderSourceBinary>("debug_draw_spv.frag")

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
