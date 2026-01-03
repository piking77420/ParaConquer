#pragma once

#include "CoreHeader.hpp"

#include "LowRenderer/CommandList.hpp"

#include "Math/ToolboxTypedef.hpp"
#include "Rendering/RenderingTypedef.h"
#include "Rendering/Buffer/UniformBuffer.hpp"
#include "Resources/GraphicShader.hpp"
#include "Resources/Mesh.hpp"
#include "Scripting/ScriptingLua.hpp"
#include "ObjectPtr.hpp"

BEGIN_PCCORE
    class PC_CORE_API DebugDrawContext
    {
    public:
        static void DrawRay(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _dir, float _distance = 1.f,
                            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void DrawSphere(const Tbx::Vector3d& _p1, float _radius = 0.5f,
                               Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void DrawBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
                            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void DrawWireSphere(const Tbx::Vector3d& _p1, float _radius = 0.5f,
                                   Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void DrawWireBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, const Tbx::Vector3d& _size,
                                Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void DrawCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, float _radius, float _height,
                                Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void DrawWireCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, float _radius, float _height,
                                    Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        void DrawDebugPrimitive(CommandList* _commandList);

        void Prepare();

        DEFAULT_COPY_MOVE_OPERATIONS(DebugDrawContext);

        DebugDrawContext(Rhi& _Rhi);

        ~DebugDrawContext() = default;

    private:
        Rhi& m_Rhi;

        static inline DebugDrawContext* m_Instance = nullptr;

        static constexpr size_t MAX_GIZMO_PRIMITIVE = 2048;
        static constexpr size_t GIZMO_BUFFER_SIZE = sizeof(Tbx::Matrix4x4f) * MAX_GIZMO_PRIMITIVE;

        using RayDataPerInstance = std::array<Tbx::Vector4f, 3>;
        static constexpr size_t MAX_RAY_COUNT = 1024;
        static constexpr size_t RAY_BUFFER_SIZE = sizeof(RayDataPerInstance) * MAX_RAY_COUNT;

        enum class PrimitiveType
        {
            Sphere,
            Box,
            Capsule,
            WireSphere,
            WireBox,
            WireCapsule,
            Count,
        };

        struct PrimitiveData
        {
            std::vector<Tbx::Matrix4x4f> matrixBuffer;
            VertexBuffer primitiveBuffer;
            IndexBuffer primitiveIndexBuffer;
            size_t primitiveCount;

            std::unique_ptr<RhiBuffer> instanceBuffer;
        };

        struct RayCastPrimitiveData
        {
            std::vector<RayDataPerInstance> rayBuffer;
            std::vector<float> rayThicknessBuffer;

            size_t rayCount;
            VertexBuffer vertexBuffer;
        };

        std::array<PrimitiveData, static_cast<size_t>(PrimitiveType::Count)> m_PrimitiveData;

        RayCastPrimitiveData m_RayPrimitiveData;

        std::unique_ptr<RhiDescriptorBindings> m_ShaderProgramDescriptorSets;

        std::unique_ptr<RhiShaderProgram> m_ShaderProgram;

        std::unique_ptr<RhiShaderProgram> m_ShaderProgramRay;

        std::unique_ptr<RhiDescriptorBindings> m_ShaderProgramDescriptorSetsRay;

        void CreatePrimitiveShaders();

        void CreateRayShaders();

        bool NeedToRender();

        void GenerateBasePrimitve(PrimitiveType _primitiveType, VertexBuffer* _vertexBuffer, IndexBuffer* _indexBuffer);

        static void PushBoxGizmo(PrimitiveType _primitiveType,
                                 const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
                                 Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void PushSphereGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, float _radius = 0.5f,
                                    Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        static void PushCapsuleGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
                                     float _radius, float _height, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));
        
        static std::string PrimitiveTypeToString(PrimitiveType _primitiveType);
    };


END_PCCORE
