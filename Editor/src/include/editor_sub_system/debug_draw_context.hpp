#pragma once

#include "editor_header.hpp"
#include "editor_sub_system.hpp"
#include "low_renderer/command_list.hpp"

#include "math/toolbox_typedef.hpp"
#include "rendering/rendering_typedef.h"
#include "rendering/buffer/uniform_buffer.hpp"
#include "scripting/scripting_lua.hpp"

BEGIN_EDITOR_PCCORE
	class Editor;

class DebugDrawContext : public EditorSubSystem
{
public:

	static void DrawLine(Tbx::Vector3f _p1, Tbx::Vector3f _p2, float _thickNess = 1.f);

	static void DrawSphere(Tbx::Vector3f _p1, float _radius = 0.5f);
	
	static void DrawBox(Tbx::Vector3f _p1, Tbx::Vector3f _size);
	
	static void DrawWireSphere(Tbx::Vector3f _p1, float _radius = 0.5f);
	
	static void DrawWireBox(Tbx::Vector3f _p1, Tbx::Vector3f _size);

	DEFAULT_COPY_MOVE_OPERATIONS(DebugDrawContext);

	void OnRender() override;
	
	DebugDrawContext(Editor& _editor);

	~DebugDrawContext() override = default;

private:
	static inline DebugDrawContext* m_Instance;

	enum struct PrimitiveType
	{
		Sphere,
		Box,
		Capsule,
		Ray,
		Count,
	};

	struct PrimitiveGpuResources
	{
		PC_CORE::UniformBuffer uniformBuffer;
		PC_CORE::ShaderProgramDescriptorSets* descriptorSet = nullptr;
		PC_CORE::ResourceRef<PC_CORE::ShaderProgram> shaderProgram;
	};
	
	std::array<PrimitiveGpuResources,static_cast<size_t>(PrimitiveType::Count)> m_DebugDrawRsource;
	
	void CreateShaders();
	
	void DrawDebugPrimitive(PC_CORE::CommandList* _commandList, const PC_CORE::RenderingContext& _renderingContext);
	
};


END_EDITOR_PCCORE