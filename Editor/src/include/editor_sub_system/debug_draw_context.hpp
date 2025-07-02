#pragma once

#include "editor_header.hpp"
#include "editor_sub_system.hpp"
#include "low_renderer/command_list.hpp"

#include "math/toolbox_typedef.hpp"
#include "rendering/rendering_typedef.h"
#include "rendering/buffer/uniform_buffer.hpp"
#include "resources/mesh.hpp"
#include "scripting/scripting_lua.hpp"

BEGIN_EDITOR_PCCORE
	class Editor;

// TO DO MOVE IT TO Core / RENDER
class DebugDrawContext : public EditorSubSystem
{
public:

	static void DrawLine(Tbx::Vector3d _p1, Tbx::Vector3d _p2, float _thickNess = 1.f);

	static void DrawSphere(Tbx::Vector3d _p1, float _radius = 0.5f, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f,1.f));
	
	static void DrawBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size);
	
	static void DrawWireSphere(Tbx::Vector3d _p1, float _radius = 0.5f);
	
	static void DrawWireBox(Tbx::Vector3d _p1, Tbx::Vector3d euler, Tbx::Vector3d _size);

	DEFAULT_COPY_MOVE_OPERATIONS(DebugDrawContext);

	void Render() override;
	
	DebugDrawContext(Editor& _editor);

	~DebugDrawContext() override = default;

private:
	static inline DebugDrawContext* m_Instance;
	
	

	static const size_t MAX_GIZMO_PRIMITIVE = 1024;
	
	struct GpuBufferGizmo
	{
		Tbx::Matrix4x4f buffer[MAX_GIZMO_PRIMITIVE];
	};

	PC_CORE::VertexBuffer vertexBufferSphere;
	
	PC_CORE::ResourceRef<PC_CORE::ShaderProgram> m_ShaderProgram;

	PC_CORE::ResourceRef<PC_CORE::Mesh> m_SphereMesh;
	
	std::vector<Tbx::Matrix4x4f> m_SphereGizmoBuffers;

	PC_CORE::ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSets;

	size_t m_SphereGizmoBufferCount = 0;
	
	void CreateShaders();
	
	void DrawDebugPrimitive(PC_CORE::CommandList* _commandList, const PC_CORE::RenderingContext& _renderingContex);

	
};


END_EDITOR_PCCORE