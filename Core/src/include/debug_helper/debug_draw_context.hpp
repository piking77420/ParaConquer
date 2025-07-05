#pragma once

#include "core_header.hpp"

#include "low_renderer/command_list.hpp"

#include "math/toolbox_typedef.hpp"
#include "rendering/rendering_typedef.h"
#include "rendering/buffer/uniform_buffer.hpp"
#include "resources/mesh.hpp"
#include "scripting/scripting_lua.hpp"

BEGIN_PCCORE

// TO DO MOVE IT TO Core / RENDER

class Renderer;

class PC_CORE_API DebugDrawContext
{
public:

	static void DrawLine(Tbx::Vector3d _p1, Tbx::Vector3d _p2, float _thickNess = 1.f);

	static void DrawSphere(Tbx::Vector3d _p1, float _radius = 0.5f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));

	static void DrawBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));
	
	static void DrawWireSphere(Tbx::Vector3d _p1, float _radius = 0.5f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));
	
	static void DrawWireBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size);

	void DrawDebugPrimitive(PC_CORE::CommandList* _commandList, const PC_CORE::RenderingContext& _renderingContex);

	void Prepare();
	
	DEFAULT_COPY_MOVE_OPERATIONS(DebugDrawContext);
	
	DebugDrawContext(Renderer* _renderer);

	~DebugDrawContext() = default;

private:
	static inline DebugDrawContext* m_Instance;
	
	Renderer* m_Renderer;

	static const size_t MAX_GIZMO_PRIMITIVE = 1024;
	
	struct GpuBufferGizmo
	{
		Tbx::Matrix4x4f buffer[MAX_GIZMO_PRIMITIVE];
	};

	enum class PrimitiveType
	{
		Sphere,
		Box,
		Capusle,
		WireSphere,
		WireBox,
		WireCapsule,
		Count,
	};

	struct PrimitiveData
	{
		std::vector<Tbx::Matrix4x4f> matrixBuffer;
		PC_CORE::ResourceRef<PC_CORE::Mesh> mesh;
		size_t primitiveCount;
		PC_CORE::VertexBuffer primitiveVertexBuffer;
	};

	std::array<PrimitiveData, static_cast<size_t>(PrimitiveType::Count)> m_PrimitiveData;
	
	PC_CORE::ResourceRef<PC_CORE::ShaderProgram> m_ShaderProgram;
	
	PC_CORE::ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSets;

	
	void CreateShaders();
	

	
};


END_PCCORE