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

	static void DrawRay(Tbx::Vector3d _p1, Tbx::Vector3d _dir, float _distance = 1.f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

	static void DrawSphere(Tbx::Vector3d _p1, float _radius = 0.5f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));

	static void DrawBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));
	
	static void DrawWireSphere(Tbx::Vector3d _p1, float _radius = 0.5f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));
	
	static void DrawWireBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

	static void DrawCapsule(Tbx::Vector3d _p1, Tbx::Vector3d euler, float _radius, float _height, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

	static void DrawWireCapsule(Tbx::Vector3d _p1, Tbx::Vector3d euler, float _radius, float _height, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

	void DrawDebugPrimitive(PC_CORE::CommandList* _commandList, const PC_CORE::RenderingContext& _renderingContex);

	void Prepare();
	
	DEFAULT_COPY_MOVE_OPERATIONS(DebugDrawContext);
	
	DebugDrawContext(Renderer* _renderer);

	~DebugDrawContext() = default;

private:
	static inline DebugDrawContext* m_Instance;
	
	Renderer* m_Renderer;

	static constexpr size_t MAX_GIZMO_PRIMITIVE = 10000;
	static constexpr size_t GIZMO_BUFFER_SIZE = sizeof(Tbx::Matrix4x4f) * MAX_GIZMO_PRIMITIVE;

	using RayDataPerInstance = std::array<Tbx::Vector4f, 3>;
	static constexpr size_t MAX_RAY_COUNT = 1024;
	static constexpr size_t RAY_BUFFER_SIZE = sizeof(RayDataPerInstance) * MAX_RAY_COUNT;

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

	struct RayCastPrimitiveData
	{
		
		std::vector<RayDataPerInstance> rayBuffer;
		std::vector<float> rayThicknessBuffer;

		size_t rayCount;
		PC_CORE::VertexBuffer vertexBuffer;
	};

	std::array<PrimitiveData, static_cast<size_t>(PrimitiveType::Count)> m_PrimitiveData;

	RayCastPrimitiveData m_RayPrimitiveData;
	
	PC_CORE::ResourceRef<PC_CORE::ShaderProgram> m_ShaderProgram;
	
	PC_CORE::ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSets;

	PC_CORE::ResourceRef<PC_CORE::ShaderProgram> m_ShaderProgramRay;
	
	PC_CORE::ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSetsRay;

	void CreatePrimitiveShaders();

	void CreateRayShaders();
	
	static void PushBoxGizmo(PrimitiveType _primitiveType,
		Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

	static void PushSphereGizmo(PrimitiveType _primitiveType, Tbx::Vector3d _p1, float _radius = 0.5f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

	static void PushCapsuleGizmo(PrimitiveType _primitiveType, Tbx::Vector3d _p1, Tbx::Vector3d euler, float _radius, float _height, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));
};


END_PCCORE